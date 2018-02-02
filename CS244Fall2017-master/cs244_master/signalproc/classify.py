import numpy as np
import matplotlib.pyplot as plt
from sklearn import svm
import pdb

ACTIVITIES = np.arange(1,6)

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

def compute_window(X, window_size = 100, func = np.mean):
    window = np.zeros(X.shape)
    for i in range(X.shape[0]):
        window[i] = func(X[max(0, i-window_size):min(X.shape[0]-1, i+window_size)], axis = 0)
    return np.concatenate((X, window), axis = 1) 

def evaluate_learner(clf, training_data, validation_data):
    training_MSE = np.mean(
                    np.where(clf.predict(training_data[:,:-1]) != training_data[:,-1], 1, 0) ** 2
                    )
    validation_MSE = np.mean(
                    np.where(clf.predict(validation_data[:,:-1]) - validation_data[:,-1], 1, 0) ** 2
                    )
    training_MAE = np.where(clf.predict(training_data[:,:-1]) != training_data[:,-1])[0].shape[0] / training_data.shape[0]
    validation_MAE = np.where(clf.predict(validation_data[:,:-1]) != validation_data[:,-1])[0].shape[0] / validation_data.shape[0]
    print("MSE", training_MSE, validation_MSE)
    print("MAE", training_MAE, validation_MAE)

def load_data(file_list):
    activity_data = []
    for activity, file in enumerate(file_list, 1):
        data = np.genfromtxt(file, delimiter=',', skip_header=0, usecols=(0, 1, 2))
        data = np.concatenate((compute_window(data), 
            np.full((data.shape[0], 1), activity, dtype=data.dtype)), 
            axis = 1)
        activity_data.append(data)
    return activity_data

def calculate_error_rate(clf, file_list, data_list):
    for file, test_data in zip(file_list, data_list):
        print(file, "error = ", np.sum(np.where(clf.predict(test_data[:,:-1]) != test_data[:,-1], 1, 0)))

def main():
    data_file_names = ["Ryan_laying.csv", "Ryan_sitting.csv", "Ryan_walking.csv", 
                    "ryan_jogging.csv", "ryan_running.csv"]
    #1. Sleeping (Lying down), 2. Sitting, 3. Walking, 4. Jogging, 5. Running
    activity_data_list = load_data(data_file_names)
    features = np.concatenate(activity_data_list, axis = 0)
    training_data, validation_data = split_data(features, seed=0)
    #train on partial data
    training_data = training_data[::10]

    #pdb.set_trace()
    clf = svm.SVC()
    clf.fit(training_data[:,:-1], training_data[:,-1])
    evaluate_learner(clf, training_data, validation_data)
    calculate_error_rate(clf, data_file_names, activity_data_list)
if __name__ == '__main__':
    main()
