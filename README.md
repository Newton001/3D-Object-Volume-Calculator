# 3D Object Volume Calculator

## Overview
The **3D Object Volume Calculator** is a Qt and VTK-based application designed to load 2D image files (JPEG, PNG, DICOM) and reconstruct their 3D volume. The application also provides visualization features within the interface and calculates object volumes for scientific and engineering purposes.

---

## Features
- Load supported image files: **JPEG**, **PNG**, **DICOM**.
- Perform preprocessing (e.g., smoothing, thresholding) on the loaded images.
- Reconstruct and visualize the 3D model.
- Calculate the volume of 3D objects.

---

## Requirements
### Dependencies
- **Qt** (5.15 or higher recommended)
- **VTK** (Version 9.0 or higher)
- **CMake** (Version 3.16 or higher)
- Compiler with **C++17** support

### Installation
```bash
# Clone the repository
git clone https://github.com/Newton001/3D-Object-Volume-Calculator.git
cd 3D-Object-Volume-Calculator

# Configure and build using CMake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run the application
./3D_Object_Volume_Calculator
```

---

## Usage
1. Launch the application.
2. Use the **Load Files** button to import image slices (e.g., `.png`, `.jpg`, `.dcm`).
3. Enable smoothing or adjust the threshold slider for preprocessing.
4. Click **Start Processing** to reconstruct and visualize the 3D object.
5. View the reconstructed model and volume in the interface.

---

## Project Structure
```plaintext
3D-Object-Volume-Calculator/
├── CMakeLists.txt          # Project configuration
├── src/
│   ├── main.cpp            # Main application entry point
│   ├── app/
│   │   ├── FileLoading.h   # File loading header
│   │   ├── FileLoading.cpp # File loading implementation
│   │   ├── VTKRenderer.h   # VTK Renderer header
│   │   ├── VTKRenderer.cpp # VTK Renderer implementation
│   └── ui/
│       ├── main.qml        # User interface
│       ├── qml.qrc         # QML resources
└── README.md               # Project documentation
```

---

## Development
### Adding New Features
1. Follow modular development.
2. Use advanced C++ techniques (e.g., smart pointers, concurrent programming).
3. Test each feature independently.

---

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contributions
Contributions are welcome! Please fork the repository and submit a pull request.

---

## Contact
For questions, suggestions, or feedback, feel free to reach out:
- **Author**: Newton
- **Email**: [Your Email Here]
- **GitHub**: [Newton001](https://github.com/Newton001)

---

## Future Improvements
- Add support for more file types.
- Optimize rendering for large datasets.
- Implement 3D volume export functionality.

