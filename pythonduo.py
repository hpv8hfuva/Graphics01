arr = [1 for _ in range(0, len(arr) + 1)]
for i in range(2, len(arr) + 1):
arr[i] = i*arr[i - 1]

for i in range(1, len(arr) + 1):
count = count + (arr[len(arr)]/ (arr[i]*arr[len(arr) - i])

count -= len(invalids)