import re

version_file = 'D:\\kodebi da ambebi\\roblox executor\\dll\\version.hpp'

# Read the version file
with open(version_file, 'r') as file:
    content = file.read()

# Regular expression to find the version components
test_pattern = r'#define VERSION_TEST (\d+)'

# Extract the current version numbers
test_version = int(re.search(test_pattern, content).group(1))

# Increment the patch version
test_version += 1

# Replace the version numbers in the content
content = re.sub(test_pattern, f'#define VERSION_TEST {test_version}', content)

# Write the updated version back to the file
with open(version_file, 'w') as file:
    file.write(content)