Overview
********

---

## Team Members

- **Ganesh Patil** – 3823129
- **Farhan Balekundri** – 3823048
- **Varadraj Dundappa Yalamalli** – 3822751
- **Esther Ürek** – 3531932

---

Please consult the assignment sheet for the implementation requirements.

You can build and flash the application via
```bash
$ west build -p always -b esp32_devkitc/esp32/procpu --sysbuild apps/04_pcp
$ west flash
$ west espressif monitor
```

TODO: Part a
************

What do you observe when executing the unmodified pcp_app? Describe how this problem arises (2-3 sentences) by using terminology from the lecture and reference to the Zephyr documentation.

In a default PIP (Priority Inheritance Protocol) implementation, a task only elevates its priority after a higher-priority task attempts to access the same resource and gets blocked. This creates two major issues: chain blocking, where a high-priority task can be delayed by multiple lower-priority tasks, and the potential for deadlocks if tasks request resources in a circular order. Unlike IPCP, PIP does not prevent deadlocks or minimize blocking time through a system-wide priority ceiling.

