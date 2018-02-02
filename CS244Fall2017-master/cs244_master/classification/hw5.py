import numpy as np
import matplotlib.pyplot as plt
from sklearn import svm

ACTIVITIES = np.arange(1,6)

def plot_xyz(time, x,y,z, activity):
    plt.figure()
    plt.plot(time, x, label='X')
    plt.plot(time, y, label='Y')
    plt.plot(time, z, label='Z')

    plt.title("activity = {}".format(activity))
    plt.xlabel('Time (Seconds)')
    plt.grid(True)
    plt.axis('tight')
    plt.legend(loc='upper left')
    
def graph_all_features(time: np.array, csv: "csv format"):
    for activity in ACTIVITIES:
        x, y, z, _ = extract_activity(csv, activity)
        plot_xyz(time, x, y, z, activity)
    plt.show(False)    

def extract_activity(csv: "csv format", activity:int):
    return csv.T[1+(activity-1)*4:5+(activity-1)*4]

def compute_window(X, window_size = 10, func = np.mean):
    window = np.zeros(X.shape)
    for i in range(X.shape[0]):
        #window[i] = np.sum(X[max(0, i-10):min(X.shape[0]-1, i+10)], axis = 0)
        window[i] = func(X[max(0, i-window_size):min(X.shape[0]-1, i+window_size)], axis = 0)
    return window

def split_data(data, seed = 0):
    ''' shuffle and split the data into a training and validation dataset
    inspired by the ml library from cs 273a
    '''
    np.random.seed(seed)
    pi = np.random.permutation(data.shape[0])
    shuffled_data = data[pi,:]
    split = int(data.shape[0]*0.8)
    training_data = data[pi,:][:split]
    validation_data = data[pi,:][split:]

    return training_data, validation_data

def calculate_mean(csv, window_size):
    features = np.empty(shape = (0, 7))
    for i in ACTIVITIES:
        activity_i = extract_activity(csv, i).T
        Xtr_i, Ytr_i = activity_i[:,:-1], activity_i[:,-1, np.newaxis]
        Xtr_mean_i = compute_window(Xtr_i, 1000, func = np.mean)
        features_i = np.concatenate((Xtr_i, Xtr_mean_i, Ytr_i), axis = 1)
        features = np.concatenate((features, features_i), axis = 0)
    return features

if __name__ == '__main__':
    csv = np.genfromtxt("takashin_[Students]assignment5_sampleXYZ.csv",
                         delimiter=',', skip_header=1)#, usecols=(0, 1, 2, 3))
    time = csv.T[0]
    #graph_all_features(time, csv)
    features = calculate_mean(csv, 10)
    training_data, validation_data = split_data(features, seed=0)
    #train on partial data
    training_data = training_data[::2]
    clf = svm.SVC()
    clf.fit(training_data[:,:-1], training_data[:,-1])
    training_MSE = np.mean(
                    (clf.predict(training_data[:,:-1]) - training_data[:,-1]) ** 2
                    )
    validation_MSE = np.mean(
                    (clf.predict(validation_data[:,:-1]) - validation_data[:,-1]) ** 2
                    )
    training_MAE = np.where(clf.predict(training_data[:,:-1]) != training_data[:,-1])[0].shape[0] / training_data.shape[0]
    validation_MAE = np.where(clf.predict(validation_data[:,:-1]) != validation_data[:,-1])[0].shape[0] / validation_data.shape[0]
    print("MSE", training_MSE, validation_MSE)
    print("MAE", training_MAE, validation_MAE)

