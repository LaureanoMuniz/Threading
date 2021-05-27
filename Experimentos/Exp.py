import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import subprocess
import random
import math 
import string
from tqdm import tqdm
def word_generator_uniform(size, chars=string.ascii_lowercase):
    return ''.join(random.choice(chars) for _ in range(size))
def char_generator_exp():
    prob = 0.5**(np.arange(1,26))
    prob = list(prob)
    prob.append(0.5**25)
    caracter = random.choices(string.ascii_lowercase, weights = reversed(prob))
    return caracter[0]

columnas = ['Tiempo de ejecucion (s) ', 'Tiempo de ejecucion (s)'
            , 'Cantidad de Threads ', 'Cantidad de Threads', 'Cantidad de archivos']

def pasarAFormato(cantThread1, cantThread2, listaDeArchivos, cantDeRuns):
    df_final = pd.DataFrame(columns=columnas)
    for i in range(0,cantDeRuns):
        comando = ['./../src/ContarPalabras', str(cantThread1), str(cantThread2)] + listaDeArchivos
        result = subprocess.run(comando, stdout=subprocess.PIPE)
        output = result.stdout.split()
        values = list(map(float, output))
        values = [values + [cantThread1, cantThread2, len(listaDeArchivos)]]
        df = pd.DataFrame(values, columns=columnas)
        df_final = df_final.append(df, ignore_index=True)
    return df_final

def generarFiles(cantPalabras, cantFiles, tamanoPalabra, experimento, distUniLetras):
    archivos = []
    for archivo in range(0,cantFiles):
        name = experimento + str(archivo)
        file = open('Data/' + name, 'w')
        tamano = random.randint(cantPalabras-cantPalabras//10, cantPalabras + cantPalabras//10)
        for i in range(0, tamano):
            palabra = ''
            if distUniLetras == True:
                palabra = word_generator_uniform(tamanoPalabra)
            else:
                palabra = char_generator_exp() + word_generator_uniform(tamanoPalabra - 1)
            file.write(palabra + '\n')
        file.close() 
        archivos.append('../Experimentos/Data/' + name)
    return np.array(archivos)

def experimento1(cantPalabras, tamanoPalabra, cantThreadsMax, cantFilesMax, step, cantRuns):
    #Mejor caso moviendo cantidad de archivos y variando entre pocas cantidades de letras
    archivos = generarFiles(cantPalabras, cantFilesMax, tamanoPalabra, 'Experimento1.', True)
    listaDeFrames = []
    for threads in tqdm(range(1, cantThreadsMax + 1), desc = 'Experimento 1'):
        for cantFiles in range(1, cantFilesMax + 1, step):
            listaDeFrames.append(pasarAFormato(threads, threads, list(archivos[:cantFiles]), cantRuns))
    fd = pd.concat([listaDeFrames[i] for i in range(len(listaDeFrames))], ignore_index=True)
    sns.set_theme(style="darkgrid")
    sns.lineplot( x=columnas[4], y=columnas[0],
             hue=columnas[2],
             data=fd).set_title("Performance en familia Uniforme de CargarMultiplesArchivos")
    plt.savefig('Experimento1Cargar.pdf',bbox_inches="tight")
    plt.clf()
    sns.lineplot( x=columnas[4], y=columnas[1],
             hue=columnas[3],
             data=fd).set_title("Performance en familia Uniforme de MaximoEnParalelo")
   
    plt.savefig('Experimento1Maximo.pdf',bbox_inches="tight")
    plt.clf()
    
def experimento2(cantPalabras, tamanoPalabra, cantThreadsMax, cantFilesMax, step, cantRuns):
    #Peor caso en letras
    archivos = generarFiles(cantPalabras, cantFilesMax, tamanoPalabra, 'Experimento2.', False)
    listaDeFrames = []
    for threads in tqdm(range(1, cantThreadsMax + 1), 'Experimento 2'):
        for cantFiles in range(1, cantFilesMax + 1, step):
            listaDeFrames.append(pasarAFormato(threads, threads, list(archivos[:cantFiles]), cantRuns))
    fd = pd.concat([listaDeFrames[i] for i in range(len(listaDeFrames))], ignore_index=True)

    sns.set_theme(style="darkgrid")
    sns.lineplot( x=columnas[4], y=columnas[0],
             hue=columnas[2],
             data=fd).set_title("Performance en familia Letras de CargarMultiplesArchivos")
    plt.savefig('Experimento2Cargar.pdf',bbox_inches="tight")
    plt.clf()
    sns.lineplot( x=columnas[4], y=columnas[1],
             hue=columnas[3],
             data=fd).set_title("Performance en familia Letras de MaximoEnParalelo")
   
    plt.savefig('Experimento2Maximo.pdf',bbox_inches="tight")
    plt.clf()

def generarArchivosExp3(cantFiles, cantPalabras, tamanoPalabra, gradoMaximo):
    archivos = []
    
    for archivo in range(0,cantFiles):
        name = 'Experimento3.' + str(archivo)
        file = open('Data/' + name, 'w')
        tamano = ((cantFiles * cantPalabras) // gradoMaximo) // (cantFiles-1)
        if archivo == cantFiles - 1:
            tamano = ((cantFiles * cantPalabras) * (gradoMaximo - 1)) // gradoMaximo 
        for i in range(0, int(tamano)):
            palabra = word_generator_uniform(tamanoPalabra)
            file.write(palabra + '\n')
        file.close() 
        archivos.append('../Experimentos/Data/' + name)
    return np.array(archivos)

def experimento3(cantPalabras, tamanoPalabra, cantThreadsMax, cantFilesMax, step, cantRuns, gradoMaximo):
    #Peor caso orden
    listaDeFrames = []
    for cantFiles in tqdm(range(2, cantFilesMax + 1, step), 'Experimento 3'):
        archivos = generarArchivosExp3(cantFiles, cantPalabras, tamanoPalabra, gradoMaximo)
        for thread in range(1, cantThreadsMax + 1):
            listaDeFrames.append(pasarAFormato(thread, thread, list(archivos[:cantFiles]), cantRuns))
    
    fd = pd.concat([listaDeFrames[i] for i in range(len(listaDeFrames))], ignore_index=True)

    sns.set_theme(style="darkgrid")
    sns.lineplot( x=columnas[4], y=columnas[0],
             hue=columnas[2],
             data=fd).set_title("Performance en familia Orden de CargarMultiplesArchivos")
    plt.savefig('Experimento3Cargar.pdf',bbox_inches="tight")
    plt.clf()
    sns.lineplot( x=columnas[4], y=columnas[1],
             hue=columnas[3],
             data=fd).set_title("Performance en familia Orden de MaximoEnParalelo")
   
    plt.savefig('Experimento3Maximo.pdf',bbox_inches="tight")
    plt.clf()
        
            
def cleanTextAndGenerateFiles(cantFiles):
    filename = 'Data/corpus0'
    file = open(filename, 'rt')
    text = file.read()
    file.close()
    words = text.split()
    table = str.maketrans('', '', string.punctuation)
    words = [w.translate(table) for w in words]
    words = [word.lower() for word in words]
    #print(len(words))
    words = np.array(words)
    list_words = np.array_split(words, cantFiles)
    list_archivos = []
    for i in range(0, len(list_words)):
        filename = 'Data/Experimento4.' + str(i)
        list_archivos.append(filename)
        file = open(filename, 'w')
        for word in list_words[i]:
            file.write(word + '\n')
        file.close()
    return list_archivos

def experimento4(cantFilesMax, cantThreadsMax, cantRuns):
    archivos = cleanTextAndGenerateFiles(cantFilesMax)
    listaDeFrames = []
    for threads in tqdm(range(1, cantThreadsMax + 1), desc = 'Experimento 4'):
        listaDeFrames.append(pasarAFormato(threads, threads, list(archivos), cantRuns))
    fd = pd.concat([listaDeFrames[i] for i in range(len(listaDeFrames))], ignore_index=True)
    sns.set_theme(style="darkgrid")
    ax = sns.catplot(data = fd, kind = 'bar', x = columnas[2], y = columnas[0], palette= 'rocket')
    ax.fig.suptitle("Performance en caso real de CargarMultiplesArchivos")
    plt.savefig('Experimento4Cargar.pdf',bbox_inches="tight")
    plt.clf()

    ax = sns.catplot(data = fd, kind = 'bar', x = columnas[3], y = columnas[1], palette= 'rocket')
    ax.fig.suptitle("Performance en caso real de MaximoEnParalelo")
    plt.savefig('Experimento4Maximo.pdf',bbox_inches="tight")
    plt.clf()

columnasExp5 = ['Tiempo de Ejecucion', 'Cantidad de threads', 'Cantidad de Inserciones', 'Algoritmo']
def formatoExp5(cantThread, cantInserciones, cantDeRuns):
    df_final = pd.DataFrame(columns=columnasExp5)
    for i in range(0,cantDeRuns):
        comando = ['./InsertarExp', str(cantThread), str(cantInserciones)]
        result = subprocess.run(comando, stdout=subprocess.PIPE)
        output = result.stdout.split()
        values = list(map(float, output))
        for i in range(0, 3):
            df = pd.DataFrame([[values[i]] + [cantThread, cantInserciones, i+1]], columns=columnasExp5)
            df_final = df_final.append(df, ignore_index=True)
    return df_final
def experimento5(cantMaxThread, initInserciones, cantMaxInserciones, step, stepThread, cantDeRuns):
    listaDeFrames = []
    for threads in tqdm(range(1,cantMaxThread+1, stepThread), desc = 'Experimento 5'):
        for cantInserciones in range(initInserciones, cantMaxInserciones, step):
            listaDeFrames.append(formatoExp5(threads, cantInserciones, cantDeRuns))
    fd_final = pd.concat([listaDeFrames[i] for i in range(len(listaDeFrames))], ignore_index=True)
    sns.set_theme(style="darkgrid")
    sns.lineplot(x=columnasExp5[2], y=columnasExp5[0],
             hue=columnasExp5[1], style = columnasExp5[3] ,
             data=fd_final).set_title("Comparacion de herramientas de sincronizacion")
    plt.savefig('Experimento5.pdf',bbox_inches="tight")
    plt.clf()


#experimento1(1000, 4, 4, 21, 5, 5)
#experimento2(1000, 4, 4, 21, 5, 5)
#experimento3(1000, 4, 4, 21, 5, 5, 10)
#experimento4(20, 4, 5)
#experimento5(10, 1, 10000, 1000, 9, 5)