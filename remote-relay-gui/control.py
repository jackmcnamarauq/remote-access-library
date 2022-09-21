import tkinter as tk
from tkinter import ttk
# import webbrowser
import requests

new = 1
on_url = "https://us01.proxy.teleduino.org/api/1.0/328.php?k=75977832B9FA59765C7C3EB33CCDB29C&r=setDigitalOutput&pin=2&output=1&expire_time=0&save=0"
off_url = "https://us01.proxy.teleduino.org/api/1.0/328.php?k=75977832B9FA59765C7C3EB33CCDB29C&r=setDigitalOutput&pin=2&output=0&expire_time=0&save=0"

def openweb(url):
    # webbrowser.open(url,new=new)
    result = requests.get(url)
    # if(result.status_code == 200): print("OK")

# root window
root = tk.Tk()
root.geometry('300x200')
root.resizable(False, False)
root.title('Button Demo')

# On button
on_button = ttk.Button(
    root,
    text='On',
    command=lambda: openweb(on_url)
)

# Off button
off_button = ttk.Button(
    root,
    text='Off',
    command=lambda: openweb(off_url)
)

on_button.pack(
    ipadx=5,
    ipady=5,
    expand=True
)

off_button.pack(
    ipadx=5,
    ipady=5,
    expand=True
)

root.mainloop()