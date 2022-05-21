make clean
make
cp Info/pdxinfo Game.pdx/
zip -r temp.zip Game.pdx/
rm -r -v Game.pdx/
make clean
mv temp.zip Chip8PD.pdx