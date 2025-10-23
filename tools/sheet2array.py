### READ ME !
# This file is hardcoded to look for gfx.bmp and
# font.bmp from the same directory it's executed

# "Config" variables
checkDirectory = "./assets/"

spriteSize = 8
mainSprtSheet = checkDirectory + "gfx.bmp"
fontSprtSheet = checkDirectory + "font.bmp"

# Global variable
palette = ["0x0000", "0x216a", "0x792a", "0x042a",
           "0xaa87", "0x62aa", "0xc618", "0xff9c",
           "0xf809", "0xfd00", "0xff45", "0x0707",
           "0x2d7f", "0x83b3", "0xfbb4", "0xfe55"]

from PIL import Image

# Honestly it's hastly made but it's not as slow as I expected
# it uses a bunch of for loop to turn the provided image into an
# array of 2D arrays, each representing 1 sprite in the
def processSprtSheet(sprtSheet: str, tgtWidth: int, tgtHeight: int):
    with Image.open(sprtSheet) as img:
        img = img.convert("RGB")
        
        spriteData = []
        
        if img.width == tgtWidth and img.height == tgtHeight:
            # Womp Womp big slow ultra nested for loop
            # You've got to love those, right ? Anyway it
            # just splits the image into it's individual sprites
            for i in range(int(tgtHeight / spriteSize)):
                for j in range( int(tgtWidth / spriteSize)):
                    # I don't know if that's efficient but
                    # it sure is easier to handle that way
                    sprite = img.crop((j * spriteSize, i * spriteSize, j * spriteSize + spriteSize, i * spriteSize + spriteSize))
                
                    hexColorMap = []
                    width, height = sprite.size
                    pixels = sprite.load()

                    # Oh no ! another nested for loop
                    # Nesting ception ? Nah it's just
                    # the most streight forward way I
                    # saw to extract the hex color value
                    # of each pixel for later use
                    for y in range(height):
                        row = []
                        for x in range(width):
                            r, g, b = pixels[x, y]
                            color = f"0x{((((r * 31 + 127) // 255) << 11) | (((g * 63 + 127) // 255) << 5) | ((b * 31 + 127) // 255)):04x}"
                            row.append(color)
                        hexColorMap.append(row)
                    
                    # Inserting the raw hex sprite into
                    # the file's sprite array :)
                    spriteData.append(hexColorMap)
        return spriteData
    
def prettyDataFormat(array: list):
    string = ""
    additionalStr = ", "
    spacer = "   "
    
    for i in range(len(array)):
        # We'll see a bit more of this because I'm lazy
        # and don't want to spend time making this more
        # versatile so it can be use recursivly or with
        # other things than sprite data
        if i != len(array) - 1: additionalStr = ", "
        else: additionalStr = ""
        
        if i != 0: spacer = "    "
        else: spacer = ""
            
        if array[i] == [["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"],
                 ["0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000","0x0000"]]:
            string += spacer + "{0}" + additionalStr + "\n\n"
        else:
            string += spacer + "{"
            additionalStr2 = ",\n"
            for y in range(len(array[i])):
                # I told you it was gona show up
                # again at some point. Didn't lissen
                if y != len(array[i]) - 1: additionalStr2 = ",\n"
                else: additionalStr2 = ""
                
                if y != 0: spacer2 = "    "
                else: spacer2 = ""
                
                tmpString = "{"
                additionalStr3 = ", "
                
                for x in range(len(array[i][y])):
                    # No way ! could that be another appearance
                    # of that piece of code ? pretty sure it is
                    if x != len(array[i][y]) - 1: additionalStr3 = ", "
                    else: additionalStr3 = ""
                    
                    tmpString += str(palette.index(array[i][y][x])) + additionalStr3
                string += spacer2 + tmpString + "}" + additionalStr2
            string += "}" + additionalStr + "\n\n"
    
    return string

# At last, time to place cram all this data into
# an header file with some more ugly code, who cares
# though ? It's only meant to be run once on a "powerful"
# machine (Relativly to the calculator).
with open("textures.h", "w") as output:
    output.write("#pragma once\n")
    output.write('#include "eadk/eadk.h"\n\n')
    tmpString = ""
    tmpSprData = []
    
    # Ugly starts here with more fore loops >:)
    # aaand useless if statements for the pretty
    # array formating, because pretty = better ofc
    output.write("// Default color palette for all sprites\n")
    for i in range(len(palette)):
        if i != len(palette) - 1:
            tmpString += palette[i] + ", "
            if i in [3, 7, 11]: tmpString += "\n     "
        else:
            tmpString += palette[i]
    output.write("const eadk_color_t defltPalette[16] = \n    {" + tmpString + "};\n\n")
    
    # More ugly shenanigans but for the main sprite sheet
    # with all the required pretty stuff in the formating
    # of the arrays ofc because... well you know why already
    output.write(f"// Sprite sheet ({mainSprtSheet.removeprefix(checkDirectory)}) array\n")
    
    tmpSprData = processSprtSheet(mainSprtSheet, 128, 64)
    tmpString = prettyDataFormat(tmpSprData)
    output.write(f"const uint8_t mainSprtSheet[(128 / {spriteSize}) * (64 / {spriteSize})][{spriteSize}][{spriteSize}] = \n" + "    {" + tmpString + "};\n\n")
    
    # More or less the same as for the main sheet what can I say
    output.write(f"// Sprite sheet ({fontSprtSheet.removeprefix(checkDirectory)}) array\n")
    
    tmpSprData = processSprtSheet(fontSprtSheet, 128, 80)
    tmpString = prettyDataFormat(tmpSprData)
    output.write(f"const uint8_t fontSprtSheet[(128 / {spriteSize}) * (80 / {spriteSize})][{spriteSize}][{spriteSize}] = \n" + "    {" + tmpString + "};")
