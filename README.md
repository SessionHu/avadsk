# AvAdsk

- Check real available size of disk for Linux.

- Input is `block special file`.

- Usage

  ```text
  Usage: ./avadsk [filename]
  ```

- Example

  ```text
  $ make all
  gcc avadsk.c -o avadsk
  $ sudo ./avadsk /dev/sdc
  /dev/sdc: 20709120 bytes written, 100.00%, 12800000.00B/s
  /dev/sdc: 20709120 bytes read, 100.00%, 13473684.21B/s
  /dev/sdc: last available index is 20709375
  ```
