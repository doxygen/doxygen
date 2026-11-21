
Command to build an image that has all the doxygen build dependencies.  Run in docker dir:

docker build -t doxygen/build .

After making source changes to doxygen, run the build:

docker run -v /path/to/docker/project/root:/docker --name doxygen_built doxygen/build
(
if already built, remove the container first:
docker container rm doxygen_built
)

To create an image of the built container:
docker container commit doxygen_built doxygen/installed

To generate documentation:
docker run --rm -v /path/to/docker/project/root:/docker -v /path/to/source_to_document:/source_to_document doxygen/installed /doxygen/docker/run.sh
(you may want to put the run.sh under /source_to_document and modify it)
