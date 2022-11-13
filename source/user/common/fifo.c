#include "fifo.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

/**
  * @brief  ע��һ��fifo
  * @param  fifo: fifo�ṹ��ָ��
		    buf: fifo�ڴ��
		    size: ����
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
  * @brief  �ͷ�fifo
  * @param  fifo: fifo�ṹ��ָ��
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
  * @brief  ��fifoд����
  * @param  fifo: fifo�ṹ��ָ��
		    buf: ��д����
		    size: ��д���ݴ�С
  * @retval ʵ��д��С
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
  * @brief  ��fifo������
  * @param  fifo: fifo�ṹ��ָ��
		    buf: �������ݻ���
		    size: �������ݴ�С
  * @retval ʵ�ʶ���С
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
  * @brief  ��ȡfifo�ռ��С
  * @param  fifo: fifo�ṹ��ָ��
  * @retval fifo��С
*/
uint32_t fifo_get_total_size(_fifo_t *fifo)
{
    if (fifo == NULL)
        return 0;

    return fifo->buf_size;
}

/**
  * @brief  ��ȡfifo���пռ��С
  * @param  fifo: fifo�ṹ��ָ��
  * @retval ���пռ��С
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
  * @brief  ��ȡfifo���ÿռ��С
  * @param  fifo: fifo�ṹ��ָ��
  * @retval fifo���ô�С
*/
uint32_t fifo_get_occupy_size(_fifo_t *fifo)
{
    if (fifo == NULL) {
        return 0;
    }

    return fifo->occupy_size;
}
