#ifndef __PCA9670_H_
#define __PCA9670_H_

int pca9670_set_val(u8 chip, uint mask, uint data);
int pca9670_get_val(u8 chip);

#endif /* __PCA9670_H_ */
