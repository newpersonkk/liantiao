
#include "param.h"

#define BUFFER_SIZE 42
#define MARKER '7'

uint8_t rxBuffer1[BUFFER_SIZE];
uint8_t rxBuffer2[BUFFER_SIZE];
uint8_t rxIndex1 = 0;
uint8_t rxIndex2 = 0;

int findNthLastMarker(const char *data, int n) {
  int count = 0;
  for (int i = BUFFER_SIZE-1; i >= 0; i--) {
    if (data[i] == MARKER) {
      count++;
      if (count == n) return i;
    }
  }
  return -1; // 未找到足够的标记
}

void ProcessData1(void) {
  const char *data = (const char*)rxBuffer1;
  
  // 查找最后三个'7'的位置
  int pos1 = findNthLastMarker(data, 1); // 最后一个'7'
  int pos2 = findNthLastMarker(data, 2); // 倒数第二个'7'
  int pos3 = findNthLastMarker(data, 3); // 倒数第三个'7'
  
  // 检查是否找到足够的标记
  if (pos1 < 0 || pos2 < 0 || pos3 < 0) return;
  
  // 计算两个字段的长度
  int len1 = pos1 - pos2 - 1;
  int len2 = pos2 - pos3 - 1;
  
  // 检查长度是否有效且相等
  if (len1 <= 0 || len2 <= 0 || len1 != len2) return;
  
  // 比较两个字段的内容
  if (memcmp(&data[pos2+1], &data[pos3+1], len1) == 0) {
    memcpy(tx_buffer4, &data[pos2+1], 6);
    flag1 = 1; 
  }
}

void ProcessData2(void) {
  const char *data = (const char*)rxBuffer2;
  
  // 查找最后三个'7'的位置
  int pos1 = findNthLastMarker(data, 1); // 最后一个'7'
  int pos2 = findNthLastMarker(data, 2); // 倒数第二个'7'
  int pos3 = findNthLastMarker(data, 3); // 倒数第三个'7'
  
  // 检查是否找到足够的标记
  if (pos1 < 0 || pos2 < 0 || pos3 < 0) return;
  
  // 计算两个字段的长度
  int len1 = pos1 - pos2 - 1;
  int len2 = pos2 - pos3 - 1;
  
  // 检查长度是否有效且相等
  if (len1 <= 0 || len2 <= 0 || len1 != len2) return;
  
  // 比较两个字段的内容
  if (memcmp(&data[pos2+1], &data[pos3+1], len1) == 0) {
    memcpy(tx_buffer5, &data[pos2+1], 6);
    flag2 = 1; 
  }
}
