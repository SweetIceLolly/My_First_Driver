# My_First_Driver
Hello world from kernel!

# [KmdfHelloWorld](KmdfHelloWorld) folder
This is my first kernel-mode driver. It does the following things:
1. Say hello to you when it is loaded
2. Handle major functions (IRP). Specifically, `IRP_MJ_CREATE`, `IRP_MJ_CLOSE`, and `IRP_MJ_WRITE`
3. Setup an IO device so you can communicate with it
4. For IRP_MJ_WRITE: the driver will obtain the received buffer and output it

The driver does nothing meaningful but it is meaningful to me :)

A valuable experience.

# [DriverTester](DriverTester) folder
This is my first driver loader. It does the following things:
1. Ask you to input the driver (.sys) file
2. Create service
3. Start service
4. Get an IO handle to the driver
5. Allow you to type the message to be sent to the driver
6. Close the IO handle
7. Stop service
8. Delete service

This driver tester is used to test the [KmdfHelloWorld](KmdfHelloWorld) driver.

Again, this program does nothing meaningful but it is meaningful to me :)

A valuable experience, too.

# Acknowledgment/Credits
I found [this helpful tutorial](https://www.unknowncheats.me/forum/c-and-c/59147-writing-drivers-perform-kernel-level-ssdt-hooking.html), which helped me a lot. I followed the tutorial to implement my programs. Thank you for the valuable and detailed tutorial! ❤

I tried to follow the tutorial to implement SSDT hook but I failed because my driver is x64 :(

I heard that `KeServiceDescriptorTable` is not exported on x64 systems, so I can't do that on my computer :(

I also tried some [methods](https://github.com/conix-security/zer0m0n/blob/master/src/driver/x64/hook.c#L96) but it didn't work :(

Anyway, this is my first driver. I hope you can find what you need in this repo :)
