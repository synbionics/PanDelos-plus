import pandas as pd

# cosa manca di file 1 in file 2
def confronta_file(file1, file2, file_out):

    # dataframes
    # df1 = pd.read_csv(file1, sep=',', header=None, names=['rowGene', 'colGene', 'score'])
    # df2 = pd.read_csv(file2, sep='\t', header=None, names=['rowGene', 'colGene', 'score'])
    
    # df1 = pd.read_csv(file1, sep='\t', header=None, names=['rowGene', 'colGene', 'score'])
    # df2 = pd.read_csv(file2, sep=',', header=None, names=['rowGene', 'colGene', 'score'])

    df1 = pd.read_csv(file1, sep=',', header=None, names=['rowGene', 'colGene', 'score'])
    df2 = pd.read_csv(file2, sep=',', header=None, names=['rowGene', 'colGene', 'score'])


    df1['score'] = df1['score'].apply(lambda x: round(x, 6))
    df2['score'] = df2['score'].apply(lambda x: round(x, 6))

    righe_mancanti = pd.merge(df2, df1, on=['rowGene', 'colGene', 'score'], how='left', indicator=True).query('_merge == "left_only"').drop('_merge', axis=1)

    righe_mancanti.to_csv(file_out, sep='\t', header=None, index=False)

confronta_file('oldMatch.net', 'parallelMatch.net', 'short.csv')
confronta_file('parallelMatch.net', 'oldMatch.net', 'shortRev.csv')
    