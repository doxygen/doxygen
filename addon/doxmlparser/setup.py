import os

from pathlib import Path
from setuptools import setup, find_packages

topdir = Path(os.getcwd()).parent.parent

with open(topdir / 'VERSION') as f:
    version = f.read()

with open('README.md') as f:
    readme = f.read()

with open('LICENSE') as f:
    license = f.read()

with open('requirements.txt') as f:
    requirements = f.read().splitlines()

setup(
    name='doxmlparser',
    version=version,
    description='Python API to access doxygen generated XML output',
    long_description=readme,
    author='Dimitri van Heesch',
    author_email='doxygen@gmail.com',
    url='https://github.com/doxygen/doxygen/addon/doxmlparser',
    license=license,
    packages=find_packages(exclude=('tests', 'docs')),
    install_requires=requirements
)
