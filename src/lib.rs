use pyo3::pymodule;
use pyo3::types::PyModule;
use pyo3::Bound;
use pyo3::PyResult;

#[pymodule]
fn _vshogi(_m: &Bound<'_, PyModule>) -> PyResult<()> {
    Ok(())
}