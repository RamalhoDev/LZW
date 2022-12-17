from os import listdir
import subprocess

def create_classification_training_dictionary(path):
    files_binary = bytearray()
    for file in listdir(path):
        file_path = path + '/' + file
        file_bin = open(file_path, "rb")
        files_binary = files_binary + file_bin.read()
    with open('test', 'wb') as f:
        f.write(files_binary)
        f.close()

def write_dictionaries(category):
    for i in range(9, 17):
        args = f"./build/iti_lzw -i test --operation compress --number_of_bits {i} --dict_path dictionaries/{category}_{i}.dict".split(' ')
        subprocess.call(args)

create_classification_training_dictionary('Tire Textures/training_data/normal')
write_dictionaries('normal')
create_classification_training_dictionary('Tire Textures/training_data/cracked')
write_dictionaries('cracked')