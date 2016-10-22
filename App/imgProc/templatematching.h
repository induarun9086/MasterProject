/*
 * templatematching.h
 *
 *  Created on: 29-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#ifndef TEMPLATEMATCHING_H_
#define TEMPLATEMATCHING_H_

#include "utils/misc.h"
#include "utils/matrix.h"
#include "fileOps/bmp.h"
#include "imgOps/imfilter.h"
#include "imgOps/imgUtils.h"
#include "imgOps/RGBToGray.h"
#include "test/test.h"

#define TEMPLATEMATCHING_IMAGE_LENGTH (10000000)

#define RUN_MODE_UNDEF           (0)
#define RUN_MODE_DO_TESTS        (1)
#define RUN_MODE_SEPERATE_OBJS   (2)
#define RUN_MODE_FIND_GN_OBJ     (3)
#define RUN_MODE_MARK_OBJS       (4)

#endif /* TEMPLATEMATCHING_H_ */
