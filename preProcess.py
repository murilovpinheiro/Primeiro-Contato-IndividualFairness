import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.preprocessing import OneHotEncoder
from sklearn.preprocessing import OrdinalEncoder


def OHE(data, categoricalLabels):
    encoder = OneHotEncoder(handle_unknown='ignore', sparse = False, categories = "auto", dtype='int8') # Criando um Codificador
        
    OHEcolumns = pd.DataFrame(encoder.fit_transform(data[categoricalLabels])) # Fazendo o treino de codificação inicial
    columns = encoder.get_feature_names_out(categoricalLabels) # Para manter o nome das Features no OneHotEncoder eu capturo o nome das Features aqui
    OHEcolumns.columns = columns # Mudo as Colunas para os Nomes capturados antes

    OHEcolumns.index = data.index # Atualizando os indices
    old_data = data[categoricalLabels] # Salva as Features antes do Pré-Processamento para utilizar depois na construção do gráfico

    OHEdata = data.drop(categoricalLabels, axis = 1) # Apaga as Features que serão substituidas pelas colunas codificadas

    OHEdata = pd.concat([OHEdata, OHEcolumns], axis = 1) # Junta o DataSet com as novas Features codificadas
        
    OHEdata['income'] = OHEdata['income'].apply(lambda p: 1 if p == '>50K' else 0) # Codifica o Y em Booleano, mantive
    return OHEdata, old_data

def ORE(data, categoricalLabels):
    encoder = OrdinalEncoder(dtype = "int16")

    OREcolumns = pd.DataFrame(encoder.fit_transform(data[categoricalLabels]))
    columns = encoder.get_feature_names_out(categoricalLabels)
    OREcolumns.columns = columns
    old_data = data[categoricalLabels]

    OREdata = data.drop(categoricalLabels, axis = 1)

    OREdata = pd.concat([OREdata, OREcolumns], axis = 1)

    OREdata['income'] = OREdata['income'].apply(lambda p: 1 if p == '>50K' else 0) # Codifica o Y em Booleano, mantive

    return OREdata, old_data

def preProcessamento(data, encode):
    race = data['race'].replace({"Black": "Unprivileged",  "Asian-Pac-Islander": "Unprivileged", "Amer-Indian-Eskimo": "Unprivileged", "Other": "Unprivileged", "White": "Privileged"})
    data = data.drop('native-country', axis = 1)
    data = data.drop('race', axis = 1)
    data = data.drop('education', axis = 1) # excluindo isso pq já temos a equivalente numérica educational-num
    categoricalLabels = ((data.drop('income', axis = 1)).select_dtypes(include = ['object'])).columns

    if(encode == "onehot"):
        data, old_data = OHE(data, categoricalLabels)
    elif(encode == "ordinal"):
        data, old_data = ORE(data, categoricalLabels)
    else:
        return data, race, old_data  
    return data, race, old_data