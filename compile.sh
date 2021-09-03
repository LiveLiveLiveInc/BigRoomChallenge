#export LDFLAGS="-L/usr/local/opt/opencv@2/lib"
#export CPPFLAGS="-I/usr/local/opt/opencv@2/include"
export PKG_CONFIG_PATH="/usr/local/opt/opencv@2/lib/pkgconfig"
brew install opencv@2 
g++ $(pkg-config --cflags --libs opencv)  main.cpp -o Test  
echo "To run use: ./Test in.jpg"
