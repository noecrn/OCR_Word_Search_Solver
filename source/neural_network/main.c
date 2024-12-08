#include "./cnn/cnn.h"
#include "./cnn/detection_load_data.h"
#include "./cnn/init.h"
#include "./cnn/log.h"
#include "./cnn/read_data.h"
#include "./cnn/read_init.h"
#include "./cnn/read_png.h"
#include "./cnn/train.h"
#include "./cnn/write_data.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() { load_data("../data"); }
