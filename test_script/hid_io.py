import hid

# for device_dict in hid.enumerate():
#     print(f"Vendor ID: {device_dict['vendor_id']:04x}")
#     print(f"Product ID: {device_dict['product_id']:04x}")
#     print(f"Manufacturer: {device_dict['manufacturer_string']}")
#     print(f"Product: {device_dict['product_string']}")
#     print(f"Path: {device_dict['path']}")
#     print("=" * 40)


import hid
import time
import numpy as np
import matplotlib.pyplot as plt

# 替换为你的设备的 Vendor ID 和 Product ID
VENDOR_ID = 0x1a86
PRODUCT_ID = 0xfe07

# 打开设备
h = hid.device()
h.open(VENDOR_ID, PRODUCT_ID)  # 
print(f"Opened device: {h.get_manufacturer_string()}")
print("Product: %s" % h.get_product_string())
print("Serial No: %s" % h.get_serial_number_string())
# print("Firmware: %s" % h.get_feature_report())
# 设置非阻塞模式（可选）
h.set_nonblocking(True)

# 读取数据
luxs_history = []
count = 0
while True:
    data = h.read(64)  # 读取最多 64 字节
    if data:
        # print("Received:", data)
        # print(type(bytes(data[:4])))
        dt = np.dtype(np.float32)
        dt = dt.newbyteorder('<')
        lux = np.frombuffer(bytes(data[:4]), dtype=dt)[0]
        luxs_history.append(lux)
        print("Lux:", lux)
        count += 1

        if count > 20:
            break


    time.sleep(0.1)
plt.plot(luxs_history)
plt.show()