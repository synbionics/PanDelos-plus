
import pandas as pd
import sys

f1 = sys.argv[1]
f2 = sys.argv[2]

def confronta_file(file1, file2, file_out):

    df1 = pd.read_csv(file1, sep='\t', header=None, names=['rowGene', 'colGene','score'])
    # df1 = pd.read_csv(file2, sep=',', header=None, names=['rowGene', 'colGene','score'])
    df2 = pd.read_csv(file2, sep=',', header=None, names=['rowGene', 'colGene','score'])

    df1['score'] = df1['score'].apply(lambda x: round(x, 6))
    df2['score'] = df2['score'].apply(lambda x: round(x, 6))

    righe_mancanti = pd.merge(df1, df2, on=['rowGene', 'colGene','score'], how='outer', indicator=True).query('_merge == "left_only"').drop('_merge', axis=1)

    righe_mancanti.to_csv(file_out, sep='\t', header=None, index=False)



confronta_file(f1, f2, 'normal.csv')
# confronta_file(f2, f1, 'reverse.csv')