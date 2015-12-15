#ifndef gpio_H
#define gpio_H
 
int gpio_export(unsigned int gpio);
int gpio_fd_close(int fd);
int gpio_fd_open(unsigned int gpio);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_unexport(unsigned int gpio);
 
#endif /* gpio_H */