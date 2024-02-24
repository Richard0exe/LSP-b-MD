def create_identical_files(content, file1, file2):
    with open(file1, 'w') as f:
        f.write(content)
    with open(file2, 'w') as f:
        f.write(content)

# Use the function
create_identical_files("This is some tex22t", "file1.txt", "folder1/file1.txt")