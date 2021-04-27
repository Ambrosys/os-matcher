rm -rf build/

docker build --no-cache -f Dockerfile-build -t ambrosys/os-matcher:docs .

echo ==========================
echo Open http://localhost:8080
echo ==========================

docker run --rm -it -p 8080:80 ambrosys/os-matcher:docs
