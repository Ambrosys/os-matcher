rm -rf build/

docker build --no-cache -f Dockerfile-build -t ambrosys/os-matcher:docs .
CONTAINER=$(docker run -d ambrosys/os-matcher:docs)

docker wait $CONTAINER
docker cp $CONTAINER:/home/build/ .
docker stop $CONTAINER
docker rm $CONTAINER