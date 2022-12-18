import os
import pandas as pd

instance = pd.read_csv("Emotion_final.csv")
emotion = 'fear'
# os.mkdir('instance/' + emotion)
for (i, row) in instance[instance['Emotion'] == emotion].iterrows():
    with open("instance/%s/%s%d.txt" % (emotion,emotion,i), 'w') as fp:
        fp.write(row["Text"])