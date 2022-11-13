#include "fifo.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

/**
  * @brief  注册一个fifo
  * @param  fifo: fifo结构体指针
		    buf: fifo内存块
		    size: 长度
  * @retval none
*/
void fifo_register(_fifo_t *fifo, uint8_t *buf, uint32_t size, lock_fun lock, lock_fun unlock)
{
    fifo->buf_size    = size;
    fifo->buf         = buf;
    fifo->pwrite      = fifo->buf;
    fifo->pread       = fifo->buf;
    fifo->occupy_size = 0;
    fifo->lock        = lock;
    fifo->unlock      = unlock;
}

/**
  * @brief  释放fifo
  * @param  fifo: fifo结构体指针
  * @retval none
*/
void fifo_release(_fifo_t *fifo)
{
    fifo->buf_size    = 0;
    fifo->occupy_size = 0;
    fifo->buf         = NULL;
    fifo->pwrite      = 0;
    fifo->pread       = 0;
    fifo->lock        = NULL;
    fifo->unlock      = NULL;
}

/**
  * @brief  往fifo写数据
  * @param  fifo: fifo结构体指针
		    buf: 待写数据
		    size: 待写数据大小
  * @retval 实际写大小
*/
uint32_t fifo_write(_fifo_t *fifo, const uint8_t *buf, uint32_t size)
{
    uint32_t w_size = 0, free_size = 0;

    if ((size == 0) || (fifo == NULL) || (buf == NULL)) {
        return 0;
    }

    free_size = fifo_get_free_size(fifo);
    if (free_size == 0) {
        return 0;
    }

    if (free_size < size) {
        size = free_size;
    }
    w_size = size;
    if (fifo->lock != NULL)
        fifo->lock();
    while (w_size-- > 0) {
        *fifo->pwrite++ = *buf++;
        if (fifo->pwrite >= (fifo->buf + fifo->buf_size)) {
            fifo->pwrite = fifo->buf;
        }
        fifo->occupy_size++;
    }
    if (fifo->unlock != NULL)
        fifo->unlock();
    return size;
}

/**
  * @brief  从fifo读数据
  * @param  fifo: fifo结构体指针
		    buf: 待读数据缓存
		    size: 待读数据大小
  * @retval 实际读大小
*/
uint32_t fifo_read(_fifo_t *fifo, uint8_t *buf, uint32_t size)
{
    uint32_t r_size = 0, occupy_size = 0;

    if ((size == 0) || (fifo == NULL) || (buf == NULL)) {
        return 0;
    }

    occupy_size = fifo_get_occupy_size(fifo);
    if (occupy_size == 0) {
        return 0;
    }

    if (occupy_size < size) {
        size = occupy_size;
    }
    if (fifo->lock != NULL)
        fifo->lock();
    r_size = size;
    while (r_size-- > 0) {
        *buf++ = *fifo->pread++;
        if (fifo->pread >= (fifo->buf + fifo->buf_size)) {
            fifo->pread = fifo->buf;
        }
        fifo->occupy_size--;
    }
    if (fifo->unlock != NULL)
        fifo->unlock();
    return size;
}

/**
  * @brief  获取fifo空间大小
  * @param  fifo: fifo结构体指针
  * @retval fifo大小
*/
uint32_t fifo_get_total_size(_fifo_t *fifo)
{
    if (fifo == NULL)
        return 0;

    return fifo->buf_size;
}

/**
  * @brief  获取fifo空闲空间大小
  * @param  fifo: fifo结构体指针
  * @retval 空闲空间大小
*/
uint32_t fifo_get_free_size(_fifo_t *fifo)
{
    uint32_t size;

    if (fifo == NULL)
        return 0;

    size = fifo->buf_size - fifo_get_occupy_size(fifo);

    return size;
}

/**
  * @brief  获取fifo已用空间大小
  * @param  fifo: fifo结构体指针
  * @retval fifo已用大小
*/
uint32_t fifo_get_occupy_size(_fifo_t *fifo)
{
    if (fifo == NULL) {
        return 0;
    }

    return fifo->occupy_size;
}
