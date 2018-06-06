# arduino_beerbottlr

Arduino code for beerbottle-filling-goodie

Hardware parts:
- 12V power supply
- food-grade liquid pump 12V
- arduino Uno
- weight cell (1 kg) with hx711 signal amplifier
- 16x2 LCD with i2c interface
- transistor for switching liquid pump


## LCD

Juiste LCD library installeren:
ga naar <sketch directory>/libraries
maak dir LiquidCrystal_I2C aan

    cd LiquidCrystal_I2C
    git clone https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
    cd Arduino-LiquidCrystal-I2C-library
    mv * ..
    cd ..
    rm -rf Arduino-LiquidCrystal-I2C-library
