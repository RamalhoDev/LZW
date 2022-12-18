import os
import subprocess
import json
import time
import tqdm
import pandas as pd
from sklearn.model_selection import train_test_split

angerX = list(map(lambda a: 'instance/anger/' + a, os.listdir('instance/anger')))
fearX = list(map(lambda a: 'instance/fear/' + a, os.listdir('instance/fear')))
happyX = list(map(lambda a: 'instance/happy/' + a, os.listdir('instance/happy')))
sadnessX = list(map(lambda a: 'instance/sadness/' + a, os.listdir('instance/sadness')))

angerY = ["anger"] * len(angerX)
fearY = ["fear"] * len(fearX)
happyY = ["happy"] * len(happyX)
sadnessY = ["sadness"] * len(sadnessX)

allX = angerX + fearX + happyX + sadnessX
allY = angerY + fearY + happyY + sadnessY

angerX_train, angerX_test, angerY_train, angerY_test =  train_test_split(angerX, angerY, test_size=0.3, random_state=42)
sadnessX_train, sadnessX_test, sadnessY_train, sadnessY_test =  train_test_split(sadnessX, sadnessY, test_size=0.3, random_state=42)
happyX_train, happyX_test, happyY_train, happyY_test =  train_test_split(happyX, happyY, test_size=0.3, random_state=42)
fearX_train, fearX_test, fearY_train, fearY_test =  train_test_split(fearX, fearY, test_size=0.3, random_state=42)
    
emotion = 'sadness'
fp = open("instance/%s_concat.txt" % (emotion), 'w')
for file in sadnessX_train:
    fp2 = open(file, 'r')
    data = fp2.readlines()
    for line in data:
        fp.write(line)

emotion = 'anger'
fp = open("instance/%s_concat.txt" % (emotion), 'w')
for file in angerX_train:
    fp2 = open(file, 'r')
    data = fp2.readlines()
    for line in data:
        fp.write(line)

emotion = 'happy'
fp = open("instance/%s_concat.txt" % (emotion), 'w')
for file in happyX_train:
    fp2 = open(file, 'r')
    data = fp2.readlines()
    for line in data:
        fp.write(line)

emotion = 'fear'
fp = open("instance/%s_concat.txt" % (emotion), 'w')
for file in fearX_train:
    fp2 = open(file, 'r')
    data = fp2.readlines()
    for line in data:
        fp.write(line)


def write_dictionaries(category, file_path):
    for i in range(9, 17):
        args = f"./build/iti_lzw -i {file_path} --operation compress --number_of_bits {i} --dict_path dictionaries/{category}/{category}_{i}.dict".split(' ')
        subprocess.call(args)

write_dictionaries('anger', "instance/anger_concat.txt")
write_dictionaries('sadness', "instance/sadness_concat.txt")
write_dictionaries('fear', "instance/fear_concat.txt")
write_dictionaries('happy', "instance/happy_concat.txt")

resultadao = list()

for i in tqdm.tqdm(range(9, 17)):
    for category in tqdm.tqdm(['anger', 'sadness', 'happy', 'fear'], leave=False):

        for file_path in tqdm.tqdm(angerX_test, leave=False):
            args = f"./build/iti_lzw -i {file_path} --operation compress --number_of_bits {i} --dict_path dictionaries/{category}/{category}_{i}.dict -o results/{category}_{i}.out".split(' ')
            subprocess.call(args)
            fp = open(f"results/{category}_{i}.out.json", 'r')
            result = json.load(fp)
            result['index'] = i
            result['fp'] = file_path
            result['cat'] = category
            resultadao.append(result)

        for file_path in tqdm.tqdm(sadnessX_test, leave=False):
            args = f"./build/iti_lzw -i {file_path} --operation compress --number_of_bits {i} --dict_path dictionaries/{category}/{category}_{i}.dict -o results/{category}_{i}.out".split(' ')
            subprocess.call(args)
            fp = open(f'results/{category}_{i}.out.json', 'r')
            result = json.load(fp)
            result['index'] = i
            result['fp'] = file_path
            result['cat'] = category
            resultadao.append(result)

        for file_path in tqdm.tqdm(fearX_test, leave=False):
            args = f"./build/iti_lzw -i {file_path} --operation compress --number_of_bits {i} --dict_path dictionaries/{category}/{category}_{i}.dict -o results/{category}_{i}.out".split(' ')
            subprocess.call(args)
            fp = open(f'results/{category}_{i}.out.json', 'r')
            result = json.load(fp)
            result['index'] = i
            result['fp'] = file_path
            result['cat'] = category
            resultadao.append(result)

        for file_path in tqdm.tqdm(happyX_test, leave=False):
            args = f"./build/iti_lzw -i {file_path} --operation compress --number_of_bits {i} --dict_path dictionaries/{category}/{category}_{i}.dict -o results/{category}_{i}.out".split(' ')
            subprocess.call(args)
            fp = open(f'results/{category}_{i}.out.json', 'r')
            result = json.load(fp)
            result['index'] = i
            result['fp'] = file_path
            result['cat'] = category
            resultadao.append(result)

result_df = pd.DataFrame(resultadao)
result_df.to_csv("result.csv")
result_df.to_pickle("result.pickle")