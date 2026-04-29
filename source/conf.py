# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'EET_BMS'
copyright = '2026, Pavel Aleinikau'
author = 'Pavel Aleinikau'
release = '2026'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

from pathlib import Path
import shutil

extensions = [
	'myst_parser',
]

source_suffix = {
	'.rst': 'restructuredtext',
	'.md': 'markdown',
}

templates_path = ['_templates']
exclude_patterns = []


def _sync_docs_into_source() -> None:
	"""Copy docs markdown files/resources into source/docs before build."""
	project_root = Path(__file__).resolve().parent.parent
	docs_src = project_root / 'docs'
	docs_dst = Path(__file__).resolve().parent / 'docs'

	if not docs_src.exists():
		return

	docs_dst.mkdir(parents=True, exist_ok=True)

	for item in docs_src.iterdir():
		if item.is_file() and item.suffix.lower() == '.md':
			shutil.copy2(item, docs_dst / item.name)
		elif item.is_dir() and item.name == 'doc_res':
			shutil.copytree(item, docs_dst / item.name, dirs_exist_ok=True)


_sync_docs_into_source()



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['_static']
