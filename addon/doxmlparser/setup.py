from setuptools import setup, find_packages

with open('README.md') as f:
    readme = f.read()

with open('LICENSE') as f:
    license = f.read()

setup(
    name='doxmlparser',
    version='0.1',
    description='Python API to access doxygen generated XML output',
    long_description=readme,
    author='Dimitri van Heesch',
    author_email='doxygen@gmail.com',
    url='https://github.com/doxygen/doxygen/addon/doxmlparser',
    license=license,
    packages=find_packages(exclude=('tests', 'docs'))
)
