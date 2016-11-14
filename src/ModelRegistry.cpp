/*
 * ModelRegistry.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: ghagleitner
 */

#include <ModelRegistry.h>

ModelRegistry::ModelRegistry() : current_(0) {
}

ModelRegistry::~ModelRegistry() {
}

void ModelRegistry::begin() {
  current_ = 0;
}

Model *ModelRegistry::current() {
  return &models[current_];
}

Model *ModelRegistry::next() {
  if(++current_ >= NUM_MODELS) {
    current_ = 0;
  }
  return &models[current_];
}

Model *ModelRegistry::previous() {
  if (--current_ >= NUM_MODELS) {
    current_ = NUM_MODELS - 1;
  }
  return &models[current_];
}
