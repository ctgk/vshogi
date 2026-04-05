const FILES = ["9", "8", "7", "6", "5", "4", "3", "2", "1"];
const RANKS = ["a", "b", "c", "d", "e", "f", "g", "h", "i"];
const HAND_ORDER = ["R", "B", "G", "S", "N", "L", "P"];
const PROMOTABLE = new Set(["R", "B", "S", "N", "L", "P"]);
const RESULT_LABELS = {
  ongoing: "Game in progress",
  black_win: "Black wins",
  white_win: "White wins",
};

const runtimeConfig = window.VSHOGI_CONFIG ?? {};
const API_ORIGIN = (runtimeConfig.apiOrigin ?? "").trim().replace(/\/$/, "");
const API_BASE = API_ORIGIN ? `${API_ORIGIN}/api` : "/api";
const TURN_LABELS = {
  black: "Black to move",
  white: "White to move",
};
const PIECE_GLYPHS = {
  K: "玉",
  R: "飛",
  B: "角",
  G: "金",
  S: "銀",
  N: "桂",
  L: "香",
  P: "歩",
  "+R": "龍",
  "+B": "馬",
  "+S": "全",
  "+N": "圭",
  "+L": "杏",
  "+P": "と",
};

const state = {
  game: null,
  selected: null,
  boardOrientation: "black",
  pendingPromotion: null,
  error: "",
  busy: false,
  playerTypes: { black: "human", white: "human" },
};

const elements = {
  board: document.querySelector("#board"),
  filesTop: document.querySelector("#filesTop"),
  filesBottom: document.querySelector("#filesBottom"),
  ranksLeft: document.querySelector("#ranksLeft"),
  ranksRight: document.querySelector("#ranksRight"),
  whiteHand: document.querySelector("#whiteHand"),
  blackHand: document.querySelector("#blackHand"),
  whiteHandSummary: document.querySelector("#whiteHandSummary"),
  blackHandSummary: document.querySelector("#blackHandSummary"),
  statusValue: document.querySelector("#statusValue"),
  statusMeta: document.querySelector("#statusMeta"),
  selectionInfo: document.querySelector("#selectionInfo"),
  errorText: document.querySelector("#errorText"),
  moveList: document.querySelector("#moveList"),
  newGameButton: document.querySelector("#newGameButton"),
  undoButton: document.querySelector("#undoButton"),
  flipButton: document.querySelector("#flipButton"),
  resignButton: document.querySelector("#resignButton"),
  promotionDialog: document.querySelector("#promotionDialog"),
  plainMoveButton: document.querySelector("#plainMoveButton"),
  promoteMoveButton: document.querySelector("#promoteMoveButton"),
  cancelPromotionButton: document.querySelector("#cancelPromotionButton"),
  blackPlayerType: document.querySelector("#blackPlayerType"),
  blackEngineConfig: document.querySelector("#blackEngineConfig"),
  blackModelPath: document.querySelector("#blackModelPath"),
  blackNumSimulations: document.querySelector("#blackNumSimulations"),
  whitePlayerType: document.querySelector("#whitePlayerType"),
  whiteEngineConfig: document.querySelector("#whiteEngineConfig"),
  whiteModelPath: document.querySelector("#whiteModelPath"),
  whiteNumSimulations: document.querySelector("#whiteNumSimulations"),
  applyPlayersButton: document.querySelector("#applyPlayersButton"),
  playersErrorText: document.querySelector("#playersErrorText"),
};

function pieceKey(piece) {
  if (!piece) {
    return "";
  }
  return piece.promoted ? `+${piece.type}` : piece.type;
}

function pieceLabel(pieceCode) {
  return PIECE_GLYPHS[pieceCode] ?? pieceCode;
}

function inBounds(row, col) {
  return row >= 0 && row < 9 && col >= 0 && col < 9;
}

function toSquare(row, col) {
  return `${FILES[col]}${RANKS[row]}`;
}

function fromSquare(square) {
  const file = square[0];
  const rank = square[1];
  return { row: RANKS.indexOf(rank), col: FILES.indexOf(file) };
}

function promotionZone(owner, row) {
  return owner === "black" ? row <= 2 : row >= 6;
}

function forward(owner) {
  return owner === "black" ? -1 : 1;
}

async function apiPost(path, body) {
  const response = await fetch(`${API_BASE}${path}`, {
    method: "POST",
    headers: body ? { "Content-Type": "application/json" } : {},
    body: body ? JSON.stringify(body) : undefined,
  });
  if (!response.ok) {
    const err = await response.json().catch(() => ({ detail: response.statusText }));
    throw new Error(err.detail ?? response.statusText);
  }
  return response.json();
}

function defaultEngineSettings() {
  return (
    window.VSHOGI_CONFIG?.defaultEngineSettings ?? {
      coeff_puct: 1.0,
      kldgain_threshold: null,
      random_rate: 0.0,
      dfpn_search_root: 0,
      dfpn_search_leaf: 0,
    }
  );
}

async function applyPlayers() {
  if (state.busy) return;
  elements.playersErrorText.textContent = "";
  state.busy = true;
  try {
    const defaults = defaultEngineSettings();
    for (const color of ["black", "white"]) {
      const isEngine = elements[`${color}PlayerType`].value === "engine";
      const tflitePath = isEngine ? elements[`${color}ModelPath`].value.trim() : null;
      const numSims = isEngine
        ? parseInt(elements[`${color}NumSimulations`].value, 10)
        : 800;
      if (isEngine && !tflitePath) {
        throw new Error(`Model path for ${color} is required.`);
      }
      await apiPost("/engine", {
        color,
        tflite_path: tflitePath,
        num_simulations: numSims,
        ...defaults,
      });
      state.playerTypes[color] = isEngine ? "engine" : "human";
    }
    state.error = "";
  } catch (err) {
    elements.playersErrorText.textContent = err.message;
    state.busy = false;
    render();
    return;
  }
  state.busy = false;
  render();
  await maybeEngineMove();
}

async function engineMove() {
  state.busy = true;
  render();
  try {
    const data = await apiPost("/engine/move");
    state.game = gameFromApi(data);
    state.selected = null;
    state.error = "";
  } catch (err) {
    state.error = `Engine error: ${err.message}`;
  } finally {
    state.busy = false;
  }
  render();
}

async function maybeEngineMove() {
  while (
    state.game &&
    state.game.result === "ongoing" &&
    state.playerTypes[state.game.turn] === "engine"
  ) {
    await engineMove();
    if (state.error) break;
  }
}

function parseSfen(sfen) {
  const parts = sfen.split(" ");
  const boardStr = parts[0];
  const turnStr = parts[1];
  const handsStr = parts[2];
  const plyStr = parts[3];

  const board = Array.from({ length: 9 }, () => Array(9).fill(null));
  const rows = boardStr.split("/");
  for (let rowIdx = 0; rowIdx < 9; rowIdx += 1) {
    let colIdx = 0;
    const row = rows[rowIdx];
    let i = 0;
    while (i < row.length) {
      const ch = row[i];
      if (ch === "+") {
        i += 1;
        const pieceCh = row[i];
        board[rowIdx][colIdx] = {
          owner: pieceCh === pieceCh.toUpperCase() ? "black" : "white",
          type: pieceCh.toUpperCase(),
          promoted: true,
        };
        colIdx += 1;
      } else if (ch >= "1" && ch <= "9") {
        colIdx += parseInt(ch, 10);
      } else {
        board[rowIdx][colIdx] = {
          owner: ch === ch.toUpperCase() ? "black" : "white",
          type: ch.toUpperCase(),
          promoted: false,
        };
        colIdx += 1;
      }
      i += 1;
    }
  }

  const turn = turnStr === "b" ? "black" : "white";

  const hands = createEmptyHands();
  if (handsStr !== "-") {
    let i = 0;
    while (i < handsStr.length) {
      let count = 0;
      while (i < handsStr.length && handsStr[i] >= "0" && handsStr[i] <= "9") {
        count = count * 10 + parseInt(handsStr[i], 10);
        i += 1;
      }
      if (count === 0) count = 1;
      const pieceCh = handsStr[i];
      const owner = pieceCh === pieceCh.toUpperCase() ? "black" : "white";
      const type = pieceCh.toUpperCase();
      hands[owner][type] = count;
      i += 1;
    }
  }

  // SFEN ply starts at 1; app ply starts at 0
  const ply = parseInt(plyStr, 10) - 1;

  return { board, turn, hands, ply };
}

function normalizeApiState(data) {
  const current = data.current ?? data;
  const sfen = current.sfen;
  const result = current.result;
  const moveHistory = data.move_history ?? current.move_history ?? [];

  if (typeof sfen !== "string" || typeof result !== "string" || !Array.isArray(moveHistory)) {
    throw new Error("Unexpected response shape from backend.");
  }

  return {
    sfen,
    result,
    moveHistory,
  };
}

function gameFromApi(data) {
  const { sfen, result, moveHistory } = normalizeApiState(data);
  const { board, turn, hands, ply } = parseSfen(sfen);
  const winner = result === "black_win" ? "black" : result === "white_win" ? "white" : null;
  const lastMoveStr = moveHistory.length > 0 ? moveHistory[moveHistory.length - 1] : null;

  return {
    board,
    hands,
    turn,
    ply,
    result,
    winner,
    moveHistory,
    lastMove: lastMoveStr ? parseMoveString(lastMoveStr) : null,
  };
}

function createEmptyHands() {
  return {
    black: Object.fromEntries(HAND_ORDER.map((piece) => [piece, 0])),
    white: Object.fromEntries(HAND_ORDER.map((piece) => [piece, 0])),
  };
}

function handSummary(hand) {
  const entries = HAND_ORDER.filter((piece) => hand[piece] > 0).map(
    (piece) => `${pieceLabel(piece)} x${hand[piece]}`,
  );
  return entries.length > 0 ? entries.join("  •  ") : "No captured pieces";
}

function shouldPromoteBeForced(piece, toRow) {
  if (piece.type === "P" || piece.type === "L") {
    return (piece.owner === "black" && toRow === 0) || (piece.owner === "white" && toRow === 8);
  }

  if (piece.type === "N") {
    return (piece.owner === "black" && toRow <= 1) || (piece.owner === "white" && toRow >= 7);
  }

  return false;
}

function goldSteps(owner) {
  const f = forward(owner);
  return [
    [f, 0],
    [f, -1],
    [f, 1],
    [0, -1],
    [0, 1],
    [-f, 0],
  ];
}

function moveSteps(piece) {
  const f = forward(piece.owner);
  const code = pieceKey(piece);

  switch (code) {
    case "K":
      return {
        step: [
          [1, 0],
          [-1, 0],
          [0, 1],
          [0, -1],
          [1, 1],
          [1, -1],
          [-1, 1],
          [-1, -1],
        ],
        slide: [],
      };
    case "G":
    case "+S":
    case "+N":
    case "+L":
    case "+P":
      return { step: goldSteps(piece.owner), slide: [] };
    case "S":
      return {
        step: [
          [f, 0],
          [f, -1],
          [f, 1],
          [-f, -1],
          [-f, 1],
        ],
        slide: [],
      };
    case "N":
      return {
        step: [
          [2 * f, -1],
          [2 * f, 1],
        ],
        slide: [],
      };
    case "L":
      return { step: [], slide: [[f, 0]] };
    case "P":
      return { step: [[f, 0]], slide: [] };
    case "R":
      return {
        step: [],
        slide: [
          [1, 0],
          [-1, 0],
          [0, 1],
          [0, -1],
        ],
      };
    case "+R":
      return {
        step: [
          [1, 1],
          [1, -1],
          [-1, 1],
          [-1, -1],
        ],
        slide: [
          [1, 0],
          [-1, 0],
          [0, 1],
          [0, -1],
        ],
      };
    case "B":
      return {
        step: [],
        slide: [
          [1, 1],
          [1, -1],
          [-1, 1],
          [-1, -1],
        ],
      };
    case "+B":
      return {
        step: [
          [1, 0],
          [-1, 0],
          [0, 1],
          [0, -1],
        ],
        slide: [
          [1, 1],
          [1, -1],
          [-1, 1],
          [-1, -1],
        ],
      };
    default:
      return { step: [], slide: [] };
  }
}

function addBoardMoves(game, row, col, piece, moves) {
  const { step, slide } = moveSteps(piece);

  for (const [dr, dc] of step) {
    const toRow = row + dr;
    const toCol = col + dc;
    if (!inBounds(toRow, toCol)) {
      continue;
    }
    const target = game.board[toRow][toCol];
    if (target && target.owner === piece.owner) {
      continue;
    }
    const fromZone = promotionZone(piece.owner, row);
    const toZone = promotionZone(piece.owner, toRow);
    const canPromote = PROMOTABLE.has(piece.type) && !piece.promoted && (fromZone || toZone);
    const forcedPromotion = !piece.promoted && shouldPromoteBeForced(piece, toRow);

    if (forcedPromotion) {
      moves.push({
        kind: "move",
        from: toSquare(row, col),
        to: toSquare(toRow, toCol),
        promote: true,
        piece: piece.type,
      });
      continue;
    }

    moves.push({
      kind: "move",
      from: toSquare(row, col),
      to: toSquare(toRow, toCol),
      promote: false,
      piece: piece.type,
    });

    if (canPromote) {
      moves.push({
        kind: "move",
        from: toSquare(row, col),
        to: toSquare(toRow, toCol),
        promote: true,
        piece: piece.type,
      });
    }
  }

  for (const [dr, dc] of slide) {
    let toRow = row + dr;
    let toCol = col + dc;

    while (inBounds(toRow, toCol)) {
      const target = game.board[toRow][toCol];
      if (target && target.owner === piece.owner) {
        break;
      }

      const fromZone = promotionZone(piece.owner, row);
      const toZone = promotionZone(piece.owner, toRow);
      const canPromote = PROMOTABLE.has(piece.type) && !piece.promoted && (fromZone || toZone);
      const forcedPromotion = !piece.promoted && shouldPromoteBeForced(piece, toRow);

      if (forcedPromotion) {
        moves.push({
          kind: "move",
          from: toSquare(row, col),
          to: toSquare(toRow, toCol),
          promote: true,
          piece: piece.type,
        });
      } else {
        moves.push({
          kind: "move",
          from: toSquare(row, col),
          to: toSquare(toRow, toCol),
          promote: false,
          piece: piece.type,
        });

        if (canPromote) {
          moves.push({
            kind: "move",
            from: toSquare(row, col),
            to: toSquare(toRow, toCol),
            promote: true,
            piece: piece.type,
          });
        }
      }

      if (target) {
        break;
      }
      toRow += dr;
      toCol += dc;
    }
  }
}

function hasUnpromotedPawnOnFile(game, owner, col) {
  for (let row = 0; row < 9; row += 1) {
    const piece = game.board[row][col];
    if (piece && piece.owner === owner && piece.type === "P" && !piece.promoted) {
      return true;
    }
  }
  return false;
}

function canDropOnRow(owner, piece, row) {
  if (piece === "P" || piece === "L") {
    if (owner === "black" && row === 0) {
      return false;
    }
    if (owner === "white" && row === 8) {
      return false;
    }
  }

  if (piece === "N") {
    if (owner === "black" && row <= 1) {
      return false;
    }
    if (owner === "white" && row >= 7) {
      return false;
    }
  }

  return true;
}

function addDropMoves(game, moves) {
  const owner = game.turn;
  const hand = game.hands[owner];

  for (const piece of HAND_ORDER) {
    if (!hand[piece]) {
      continue;
    }

    for (let row = 0; row < 9; row += 1) {
      for (let col = 0; col < 9; col += 1) {
        if (game.board[row][col]) {
          continue;
        }
        if (!canDropOnRow(owner, piece, row)) {
          continue;
        }
        if (piece === "P" && hasUnpromotedPawnOnFile(game, owner, col)) {
          continue;
        }

        moves.push({
          kind: "drop",
          piece,
          to: toSquare(row, col),
          promote: false,
        });
      }
    }
  }
}

function legalMoves(game) {
  if (game.result !== "ongoing") {
    return [];
  }

  const moves = [];
  for (let row = 0; row < 9; row += 1) {
    for (let col = 0; col < 9; col += 1) {
      const piece = game.board[row][col];
      if (!piece || piece.owner !== game.turn) {
        continue;
      }
      addBoardMoves(game, row, col, piece, moves);
    }
  }

  addDropMoves(game, moves);
  return moves;
}

function moveToString(move) {
  if (move.kind === "drop") {
    return `${move.piece}*${move.to}`;
  }
  return `${move.from}${move.to}${move.promote ? "+" : ""}`;
}

function selectableMoves() {
  if (!state.game || !state.selected) {
    return [];
  }

  const moves = legalMoves(state.game);
  if (state.selected.kind === "board") {
    return moves.filter((move) => move.kind === "move" && move.from === state.selected.square);
  }

  return moves.filter((move) => move.kind === "drop" && move.piece === state.selected.piece);
}

function describeSelection() {
  if (!state.selected) {
    return "Select a piece on the board or in hand to see legal targets.";
  }

  const moves = selectableMoves();
  if (state.selected.kind === "board") {
    return moves.length > 0
      ? `${state.selected.square} selected. ${moves.length} legal target${
          moves.length === 1 ? "" : "s"
        } available.`
      : `${state.selected.square} has no legal moves.`;
  }

  return moves.length > 0
    ? `${pieceLabel(state.selected.piece)} in hand selected. ${moves.length} drop target${
        moves.length === 1 ? "" : "s"
      } available.`
    : `${pieceLabel(state.selected.piece)} cannot be dropped right now.`;
}

function axisCell(value) {
  const element = document.createElement("div");
  element.textContent = value;
  return element;
}

function renderAxes() {
  const files = state.boardOrientation === "black" ? FILES : [...FILES].reverse();
  const ranks = state.boardOrientation === "black" ? RANKS : [...RANKS].reverse();

  elements.filesTop.replaceChildren(...files.map(axisCell));
  elements.filesBottom.replaceChildren(...files.map(axisCell));
  elements.ranksLeft.replaceChildren(...ranks.map(axisCell));
  elements.ranksRight.replaceChildren(...ranks.map(axisCell));
}

function createPieceTile(piece, selected) {
  const tile = document.createElement("div");
  tile.className = "piece-tile";
  if (piece.owner === "white") {
    tile.classList.add("is-white");
  }
  if (selected) {
    tile.classList.add("is-selected");
  }

  const glyph = document.createElement("span");
  glyph.className = "piece-glyph";
  glyph.textContent = pieceLabel(pieceKey(piece));

  const code = document.createElement("span");
  code.className = "piece-code";
  code.textContent = pieceKey(piece);

  tile.append(glyph, code);
  return tile;
}

function renderBoard() {
  const renderedFiles = state.boardOrientation === "black" ? FILES : [...FILES].reverse();
  const renderedRanks = state.boardOrientation === "black" ? RANKS : [...RANKS].reverse();
  const rows =
    state.boardOrientation === "black"
      ? state.game.board
      : [...state.game.board].reverse().map((row) => [...row].reverse());

  const targetSquares = new Set(selectableMoves().map((move) => move.to));
  const lastMove = state.game.lastMove;

  const squares = rows.flatMap((row, rowIndex) =>
    row.map((piece, colIndex) => {
      const squareId =
        state.boardOrientation === "black"
          ? toSquare(rowIndex, colIndex)
          : toSquare(8 - rowIndex, 8 - colIndex);

      const square = document.createElement("button");
      square.type = "button";
      square.className = "square";
      square.dataset.square = squareId;
      square.addEventListener("click", () => onSquareClick(squareId));

      if (state.selected?.kind === "board" && state.selected.square === squareId) {
        square.classList.add("is-selected");
      }
      if (targetSquares.has(squareId)) {
        square.classList.add("is-target");
      }
      if (lastMove && lastMove.to === squareId) {
        square.classList.add("is-last-move");
      }
      if (piece && piece.owner === state.game.turn) {
        square.classList.add("is-own-piece");
      }

      if (piece) {
        square.appendChild(
          createPieceTile(
            piece,
            state.selected?.kind === "board" && state.selected.square === squareId,
          ),
        );
      }

      if (!piece && !targetSquares.has(squareId)) {
        square.title = `${renderedFiles[colIndex]}${renderedRanks[rowIndex]}`;
      }

      return square;
    }),
  );

  elements.board.replaceChildren(...squares);
}

function renderHand(owner) {
  const hand = state.game.hands[owner];
  const container = owner === "white" ? elements.whiteHand : elements.blackHand;
  const summary = owner === "white" ? elements.whiteHandSummary : elements.blackHandSummary;
  summary.textContent = handSummary(hand);

  const children = HAND_ORDER.map((pieceType) => {
    const count = hand[pieceType];
    const selected = state.selected?.kind === "hand" && state.selected.piece === pieceType;
    const clickable = count > 0 && owner === state.game.turn && state.game.result === "ongoing";

    const button = document.createElement("button");
    button.type = "button";
    button.className = "hand-piece";
    if (clickable) {
      button.classList.add("is-clickable");
    }
    if (selected) {
      button.classList.add("is-selected");
    }

    button.disabled = !clickable;
    button.addEventListener("click", () => onHandPieceClick(owner, pieceType));

    const tile = createPieceTile(
      {
        owner,
        type: pieceType,
        promoted: false,
      },
      selected,
    );

    const countPill = document.createElement("span");
    countPill.className = "count-pill";
    countPill.textContent = `x${count}`;

    button.append(tile, countPill);
    return button;
  });

  container.replaceChildren(...children);
}

function renderMoves() {
  const items = [];
  for (let i = 0; i < state.game.moveHistory.length; i += 2) {
    const blackMove = state.game.moveHistory[i];
    const whiteMove = state.game.moveHistory[i + 1];
    const li = document.createElement("li");
    li.textContent = `${i / 2 + 1}. ${blackMove}${whiteMove ? `  ${whiteMove}` : ""}`;
    items.push(li);
  }
  elements.moveList.replaceChildren(...items);
}

function renderStatus() {
  elements.statusValue.textContent = RESULT_LABELS[state.game.result] ?? state.game.result;
  const isEngineTurn =
    state.game.result === "ongoing" &&
    state.playerTypes[state.game.turn] === "engine";
  let turnLabel;
  if (state.game.result !== "ongoing") {
    turnLabel = "";
  } else if (state.busy && isEngineTurn) {
    turnLabel = "Engine thinking\u2026";
  } else {
    const suffix = isEngineTurn ? " (engine)" : "";
    turnLabel = `${state.game.turn === "black" ? "Black" : "White"}${suffix} to move`;
  }
  elements.statusMeta.textContent = turnLabel
    ? `${turnLabel} \u2022 Ply ${state.game.ply}`
    : `Ply ${state.game.ply}`;
  elements.selectionInfo.textContent = describeSelection();
  elements.errorText.textContent = state.error;

  elements.undoButton.disabled = state.game.moveHistory.length === 0 || state.busy;
  elements.resignButton.disabled = state.game.result !== "ongoing" || state.busy;
}

function render() {
  if (!state.game) {
    elements.errorText.textContent = state.error;
    return;
  }
  renderAxes();
  renderBoard();
  renderHand("white");
  renderHand("black");
  renderMoves();
  renderStatus();
}

async function startNewGame() {
  if (state.busy) return;
  state.busy = true;
  try {
    const data = await apiPost("/game");
    state.game = gameFromApi(data);
    state.selected = null;
    state.pendingPromotion = null;
    state.error = "";
  } catch (err) {
    state.error = `Failed to start game: ${err.message}`;
  } finally {
    state.busy = false;
  }
  render();
  await maybeEngineMove();
}

async function undoMove() {
  if (!state.game || state.game.ply === 0 || state.busy) return;

  state.busy = true;
  try {
    let data = await apiPost("/undo");
    state.game = gameFromApi(data);

    // When one player is human, keep undoing until it's the human's turn
    const hasHuman =
      state.playerTypes.black === "human" || state.playerTypes.white === "human";
    while (
      hasHuman &&
      state.game.ply > 0 &&
      state.playerTypes[state.game.turn] === "engine"
    ) {
      data = await apiPost("/undo");
      state.game = gameFromApi(data);
    }

    state.selected = null;
    state.error = "";
  } catch (err) {
    state.error = err.message;
  } finally {
    state.busy = false;
  }
  render();
  await maybeEngineMove();
}

function parseMoveString(move) {
  if (move.includes("*")) {
    const [piece, to] = move.split("*");
    return { kind: "drop", piece, to, promote: false };
  }

  return {
    kind: "move",
    from: move.slice(0, 2),
    to: move.slice(2, 4),
    promote: move.endsWith("+"),
  };
}

function onHandPieceClick(owner, pieceType) {
  if (!state.game || state.game.result !== "ongoing" || state.busy) {
    return;
  }
  if (state.playerTypes[state.game.turn] === "engine") return;
  if (owner !== state.game.turn || state.game.hands[owner][pieceType] === 0) {
    return;
  }

  if (state.selected?.kind === "hand" && state.selected.piece === pieceType) {
    state.selected = null;
  } else {
    state.selected = { kind: "hand", piece: pieceType };
  }

  state.error = "";
  render();
}

function promptPromotion(plainMove, promotedMove) {
  return new Promise((resolve) => {
    state.pendingPromotion = { plainMove, promotedMove, resolve };
    elements.promotionDialog.showModal();
  });
}

function closePromotion(choice) {
  if (!state.pendingPromotion) {
    return;
  }

  const pending = state.pendingPromotion;
  state.pendingPromotion = null;
  elements.promotionDialog.close();

  if (choice === "promote") {
    pending.resolve(pending.promotedMove);
    return;
  }
  if (choice === "plain") {
    pending.resolve(pending.plainMove);
    return;
  }

  pending.resolve(null);
}

async function tryPlayMoveTo(square) {
  if (state.busy) return false;
  if (state.playerTypes[state.game.turn] === "engine") return false;
  const moves = selectableMoves().filter((move) => move.to === square);
  if (moves.length === 0) {
    return false;
  }

  let chosen;
  if (moves.length === 1) {
    [chosen] = moves;
  } else {
    const plain = moves.find((move) => !move.promote);
    const promoted = moves.find((move) => move.promote);
    chosen = await promptPromotion(plain, promoted);
    if (!chosen) {
      state.error = "Move canceled.";
      render();
      return true;
    }
  }

  state.busy = true;
  try {
    const moveStr = moveToString(chosen);
    const data = await apiPost("/move", { move: moveStr });
    state.game = gameFromApi(data);
    state.selected = null;
    state.error = "";
  } catch (err) {
    state.error = err.message;
  } finally {
    state.busy = false;
  }
  render();
  await maybeEngineMove();
  return true;
}

async function onSquareClick(square) {
  if (!state.game || state.game.result !== "ongoing") {
    return;
  }

  if (state.pendingPromotion) {
    return;
  }

  if (state.busy) return;
  if (state.playerTypes[state.game.turn] === "engine") return;

  if (state.selected && (await tryPlayMoveTo(square))) {
    return;
  }

  const { row, col } = fromSquare(square);
  const piece = state.game.board[row][col];

  if (piece && piece.owner === state.game.turn) {
    state.selected = { kind: "board", square };
    state.error = "";
  } else {
    state.selected = null;
  }

  render();
}

async function resignCurrentPlayer() {
  if (!state.game || state.game.result !== "ongoing" || state.busy) {
    return;
  }

  const loser = state.game.turn;
  state.busy = true;
  try {
    const data = await apiPost("/resign");
    state.game = gameFromApi(data);
    state.selected = null;
    state.error = `${loser === "black" ? "Black" : "White"} resigned.`;
  } catch (err) {
    state.error = err.message;
  } finally {
    state.busy = false;
  }
  render();
}

elements.newGameButton.addEventListener("click", () => {
  startNewGame();
});

elements.undoButton.addEventListener("click", () => {
  undoMove();
});

elements.flipButton.addEventListener("click", () => {
  state.boardOrientation = state.boardOrientation === "black" ? "white" : "black";
  render();
});

elements.resignButton.addEventListener("click", () => {
  resignCurrentPlayer();
});

elements.plainMoveButton.addEventListener("click", () => closePromotion("plain"));
elements.promoteMoveButton.addEventListener("click", () => closePromotion("promote"));
elements.cancelPromotionButton.addEventListener("click", () => closePromotion("cancel"));

elements.promotionDialog.addEventListener("cancel", (event) => {
  event.preventDefault();
  closePromotion("cancel");
});

elements.blackPlayerType.addEventListener("change", () => {
  elements.blackEngineConfig.classList.toggle(
    "is-hidden",
    elements.blackPlayerType.value !== "engine",
  );
});

elements.whitePlayerType.addEventListener("change", () => {
  elements.whiteEngineConfig.classList.toggle(
    "is-hidden",
    elements.whitePlayerType.value !== "engine",
  );
});

elements.applyPlayersButton.addEventListener("click", () => {
  applyPlayers();
});

window.addEventListener("keydown", (event) => {
  if (event.key === "Escape" && !state.pendingPromotion) {
    state.selected = null;
    state.error = "";
    render();
  }
});

startNewGame();
