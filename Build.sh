make clean
make
cp -r Roms/PONG.bin Game.pdx/
cp -r Roms/BREAK.bin Game.pdx/
cp -r Roms/MISSILE.bin Game.pdx/
cp -r Roms/TestRom.bin Game.pdx/
cp -r Roms/SPACE.bin Game.pdx/
zip -r temp.zip Game.pdx/
rm -r -v Game.pdx/
make clean
mv temp.zip Chip8PD.pdx