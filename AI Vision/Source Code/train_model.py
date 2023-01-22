"""
Project 34338 Telecommunication programming projects with Arduino
Code for training a model to recognize traffic signs Stop, Right, Left, Pedestrian.
Main responsible: Steffen Holm Cordes, s184208
"""

import matplotlib.pyplot as plt
import numpy as np
import tensorflow as tf
from tensorflow.keras import Model
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import (
    Dense, InputLayer, Dropout, Conv1D, Flatten, Reshape, MaxPooling1D, BatchNormalization,
    Conv2D, GlobalMaxPooling2D, Lambda, GlobalAveragePooling2D)
from tensorflow.keras.preprocessing.image import ImageDataGenerator

# Args:
SHUFFLE_DATA = False
SEED = 42
BATCH_SIZE = 32
EPOCHS = 160
# How many epochs we will fine tune the model
FINE_TUNE_EPOCHS = 60
LEARNING_RATE = 0.0005
FINE_TUNE_LEARNING_RATE = 0.000045
INPUT_SHAPE = (96, 96, 3)
WEIGHTS_PATH = './Pre-trained_weights/mobilenet_2_5_128_tf.h5'
MODEL_PATH = 'model/'
tf.random.set_seed(SEED)
np.random.seed(SEED)

# Setup Data
train_dir = 'data/training/'
test_dir = 'data/testing/'

# Image generators with rescaling of values [0-1] and data augmentation for training
train_image_generator = ImageDataGenerator(validation_split=0.2, rescale=1./255, brightness_range=[0.8,1.2],width_shift_range=0.2, height_shift_range=0.2,rotation_range=10, zoom_range=0.1)
test_image_generator = ImageDataGenerator(rescale=1./255)

# Datasets from the directories
train_dataset = train_image_generator.flow_from_directory(batch_size=BATCH_SIZE, directory=train_dir,shuffle=SHUFFLE_DATA,target_size=INPUT_SHAPE[0:2],class_mode='categorical',subset='training')
validation_dataset = train_image_generator.flow_from_directory(batch_size=BATCH_SIZE, directory=train_dir,shuffle=SHUFFLE_DATA,target_size=INPUT_SHAPE[0:2],class_mode='categorical', subset='validation')
test_dataset = test_image_generator.flow_from_directory(batch_size=BATCH_SIZE, directory=test_dir,shuffle=SHUFFLE_DATA,target_size=INPUT_SHAPE[0:2],class_mode='categorical')

# Create the base model from the pre-trained model MobileNet V1
base_model = tf.keras.applications.MobileNet(input_shape=INPUT_SHAPE, weights=WEIGHTS_PATH, alpha=0.25)

# Don't include the base model's input and top layers
no_top_model = Sequential()
no_top_model.add(InputLayer(input_shape=INPUT_SHAPE))
for layer in base_model.layers[1:-2]:
    no_top_model.add(layer)
no_top_model.trainable = False

# Setup Model
x=no_top_model.output
x=Flatten()(x)
output=Dense(5,activation='softmax')(x)
model=Model(inputs=no_top_model.input,outputs=output)

# Show model
model.summary()

# Compile the model
model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=LEARNING_RATE), loss='categorical_crossentropy', metrics=['accuracy'])

# Train the model
print("Train the model")
history = model.fit(train_dataset, validation_data=validation_dataset, epochs=EPOCHS)

# Testing model
print("Evaluate on test data")
results = model.evaluate(test_dataset, batch_size=BATCH_SIZE)
print("test loss, test acc:", results)


# Show the training and test results
train_loss = history.history['loss']
val_loss = history.history['val_accuracy']

epochs = range(1, len(train_loss) + 1)

plt.plot(epochs, train_loss, 'r', label='Training loss')
plt.plot(epochs, val_loss, 'b', label='Validation accuracy')
plt.plot(max(epochs), results[1],'k*' , label='Test accuracy')
plt.text(max(epochs)-3, results[1]+0.07, str(round(results[1]*100,2)) + "%")
plt.title('Training and validation accuracy')
plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.legend()
plt.show()

# Fine-tuning of the model, we will fine tune the last 3 of the layers
TRAINABLE_LAYERS = 3
for layer in model.layers[:-TRAINABLE_LAYERS]:
    layer.trainable=False
for layer in model.layers[-TRAINABLE_LAYERS:]:
    layer.trainable=True


# Show model
model.summary()

# compile and fine tune the model
model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=FINE_TUNE_LEARNING_RATE),loss='categorical_crossentropy',metrics=['accuracy'])
print("Fine-tuning of the model")
fine_history = model.fit(train_dataset,epochs=FINE_TUNE_EPOCHS,validation_data=validation_dataset)

# Testing model
print("Evaluate on test data")
fine_results = model.evaluate(test_dataset, batch_size=BATCH_SIZE)
print("test loss, test acc:", fine_results)

# Show the Fine-tuning results
train_loss = fine_history.history['loss']
val_loss = fine_history.history['val_accuracy']

epochs = range(1, len(train_loss) + 1)

plt.plot(epochs, train_loss, 'r', label='Training loss')
plt.plot(epochs, val_loss, 'b', label='Validation accuracy')
plt.plot(max(epochs), fine_results[1],'k*' , label='Test accuracy')
plt.text(max(epochs)-3, fine_results[1]+0.01, str(round(fine_results[1]*100,2)) + "%")
plt.title('FineTune training and validation accuracy')
plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.legend()
plt.show()

# Save model
tf.saved_model.save(model, MODEL_PATH)

# Calculating statistics for quantization
test_dataset = test_image_generator.flow_from_directory(batch_size=test_dataset.samples, directory=test_dir,shuffle=SHUFFLE_DATA,target_size=INPUT_SHAPE[0:2],class_mode='categorical')
images, labels = test_dataset.next()
minimum = np.interp(np.min(images),[0,1],[0,255])
maximum = np.interp(np.max(images),[0,1],[0,255])
mean = np.interp(np.mean(images),[0,1],[0,255])
std = np.interp(np.std(images),[0,1],[0,255])
print("Test set statistics: ", "min: ", minimum, " max: ", maximum, " mean: ", mean, " std: ", std)

# Converting a SavedModel to a Quantized TensorFlow Lite model Leaving the input/output to float.
converter = tf.lite.TFLiteConverter.from_saved_model(MODEL_PATH)
test_dataset = test_image_generator.flow_from_directory(batch_size=1, directory=test_dir,shuffle=SHUFFLE_DATA,target_size=INPUT_SHAPE[0:2],class_mode='categorical')
def representative_dataset():
  for i in range(test_dataset.samples):
    image, label = test_dataset.next()
    yield [np.array(image, dtype=np.float32, ndmin=2)]
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.experimental_new_quantizer=True
converter.default_ranges_stats=[minimum,maximum]
converter.quantized_input_stats={"input_2":[mean,std]}
converter.representative_dataset = representative_dataset
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.inference_type=tf.uint8
converter.experimental_new_converter=True
converter.experimental_new_quantizer=True
tflite_quant_model = converter.convert()

# Analyzing the converted model to make sure it is quantized and only Input/Outout layers are float32
tf.lite.experimental.Analyzer.analyze(model_content=tflite_quant_model)

# Save the quantized model.
open(MODEL_PATH + "model_quant_tl.tflite", "wb").write(tflite_quant_model)

# Load TFLite model and allocate tensors.
interpreter = tf.lite.Interpreter(model_path=MODEL_PATH + "model_quant_tl.tflite")
interpreter.allocate_tensors()
# Get input and output tensors
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Test the model on the test set.
test_dataset = test_image_generator.flow_from_directory(batch_size=1, directory=test_dir,shuffle=SHUFFLE_DATA,target_size=INPUT_SHAPE[0:2],class_mode='categorical')
correct_predictions = 0
for t in range(test_dataset.samples):
    input_data, input_label = test_dataset.next()
    interpreter.set_tensor(input_details[0]['index'], input_data)
    interpreter.invoke()
    output_data = interpreter.get_tensor(output_details[0]['index'])
    if np.argmax(output_data) == np.argmax(input_label):
        correct_predictions += 1
print("Accuracy for quantized model on test set: ", correct_predictions/test_dataset.samples)

# To convert the model to a hex dump header file, run this in linux:
#echo "const unsigned char model[] = {" > model_quant_tl.h
#cat model_quant_tl.tflite | xxd -i      >> model_quant_tl.h
#echo "};"                              >> model_quant_tl.h
# The resulting model is in the "model/Hex dump model/" folder

# To convert the model to a C source file, following the instructions from TensorFlow Lite for Microcontrollers:
# https://github.com/tensorflow/tflite-micro
# The resulting model is in the "model/C source model/" folder

# Alternatively using the TFLite Micro compiler from:
# https://github.com/on-device-ai/tflite-micro-compiler
# The resulting model is in the "model/Compiled model/" folder
