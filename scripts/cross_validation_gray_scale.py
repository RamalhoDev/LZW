from PIL import Image
from os import listdir

def grayscale_image_and_clear_metadata(path):
    for file in listdir(path):
        file_path = path + '/' + file
        img = Image.open(file_path).convert('L')
        new_img = Image.new(img.mode, img.size)
        new_img.putdata(list(img.getdata()))
        new_img.save(file_path)

grayscale_image_and_clear_metadata('/home/rodrigo/Desktop/LZW/Tire Textures/training_data/cracked')
grayscale_image_and_clear_metadata('/home/rodrigo/Desktop/LZW/Tire Textures/training_data/normal')
grayscale_image_and_clear_metadata('/home/rodrigo/Desktop/LZW/Tire Textures/testing_data/cracked')    
grayscale_image_and_clear_metadata('/home/rodrigo/Desktop/LZW/Tire Textures/testing_data/normal')
