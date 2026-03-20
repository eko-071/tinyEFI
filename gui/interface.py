import tkinter as tk


def BtnClick():
    label = tk.Label(root, text="Error")
    label.pack()


root = tk.Tk()  # the interface object
root.title("My App")  # application heading

button1 = tk.Button(root, text="ClickMe", padx=4, pady=5, command=BtnClick)
button1.pack()

label1 = tk.Label(root, text="hello")
label2 = tk.Label(root, text="Walter White")

# label1.grid(row=0, column=0)
# label2.grid(row=1, column=0)

label1.pack()
label2.pack()

root.mainloop()  # run the object
