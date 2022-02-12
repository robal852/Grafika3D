#  Reading  Wavefront OBJ files

To zasadniczo nie jest nowe ćwiczenie, ale uzupełnienie ćwiczenia Textures umożliwiające wczytywanie plików w formacie  [Wavefront OBJ](http://paulbourke.net/dataformats/obj/) i stowarzyszonych plików [Wavefront Material Template Library (MTL)](http://paulbourke.net/dataformats/mtl/). Dlatego proszę nicego nie kopiować, tylko pracować dalej na ćwiczeniu Textures. 

Proszę zacząć od  zaktualizowania swoich repozytoriów do mojego. Powinny się po tym u Państwa pojawić  kartoteki:
```
src/ObjReader
src/Geometry
src/3rdParty/tinyobjloader
```
W kartotece `src/Engine` powinny się pojawić pliki `mesh_loader.h` i `mesh_loader.cpp`. Ponadto w kartotece `Models` powinny się pojawić pliki 
`pyramid.obj`, `pyramid.mtl`, `blue_marble.obj`, `blue_marble.mtl` i `world.topo.200412.3x5400x2700.jpg`. 

 Wszystko powinno się teraz skompilować i działać jak poprzednio. 
 
Koljnym krokiem jest modyfikacja klasy  `Mesh` tak aby współdziałała z funkcją 
```c++
Mesh* load_mesh_from_obj(std::string path, std::string mtl_dir);
```
Proszę zacząć od dodania plikóœ `mesh_loader.h` i `mesh_loader.cpp`  do kompilacji w pliku `Engine/CMakeLists.txt`. Teraz kompilacja się nie powiedzie ponieważ brakuje potrzebny metod w klasie `Mesh` oraz funkcji  
```c++
GLuint create_texture(const std::string &name);
```
Zaczniemy od dodania deklarcji tych funkcji. W klasie `Mesh` dodajemy deklaracje
```c++
void *map_vertex_buffer();
void unmap_vertex_buffer();
void *map_index_buffer();
void unmap_index_buffer();
```
a w pliku `Material.h` w przestrzeni nazw `xe` deklarację
```c++
  GLuint create_texture(const std::string &name);
```
Teraz znów wszytsko powinno się kompilować pomimo braku definicji tych funkcji, a to dlatego, że na razie z nich nie korzystamy. Żeby z nich skorzystać w  metodzie `init` dodamy polecenie wczytania piramidki z pliku:
```c++
<nazwa siatki> = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/pyramid.obj",
                                          std::string(ROOT_DIR) + "/Models");
```
Za `<nazwa siatki>`  należy podstawić nazwę zmiennej do której Państwo przypisywali siatkę.  To polecenie powinno zostać dodane już poza ostatnią linijką Państwa kodu tworzącego  siatkę, tak aby _nadpisywało_ poprzednią definciję. Na koniec należy dodać
```c++
#include "Engine/mesh_loader.h"
```
Po tych zmianach pownniśmy mieć wyłacznie błedy linkowania  ponieważ nie dodaliśmy definicji potrzebnych metod i funkcji. 

W  pliku `Engine/Mesh.cpp` dodajemy więc
```c++
void *xe::Mesh::map_vertex_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void xe::Mesh::unmap_vertex_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void *xe::Mesh::map_index_buffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void xe::Mesh::unmap_index_buffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}
```
Te funkcje służą do _mapowania_ i omapywania buforów OpenGL. Za pomocą funkcji `glMapBuffer` możemy dostać  wskaźnik na  początek bufora i zapisywać dane jak do zwykłej tablicy. Jest to przydatne gdy np. chcemy zapisywać dane bezpośrednio z pliku bez ładowania ich  najpierw do tablicy, lub jak w tym przypadku  zapisujemy dane które nie są ciągłe. Należy pamiętać, żeby odmapować bufor zanim go potem użyjemy. 

I na koniec dodajemy  definicję funkcji
```c++
 GLuint create_texture(const std::string &name) {

    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;
    auto img = stbi_load(name.c_str(), &width, &height, &channels, 0);
    if (!img) {
        spdlog::warn("Could not read image from file `{}'", name);
        return 0;
    }
    GLenum format;
    if (channels == 3)
        format = GL_RGB;
    else if (channels == 4) {
        format = GL_RGBA;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0u);

    return texture;
    }
```
wewnątrz przestrzeni nazw `xe` w  pliku `Engine/Material.cpp`. Żeby ten plik się skompilował należy jeszcze dodać includa
```c++
#include "3rdParty/stb/stb_image.h"
```

Ostatnia rzecz jak nam została to dodanie w pliku `Engine/CMakeLists.txt` polecenia
```cmake
target_link_libraries(engine PUBLIC objreader)
```
Teraz już wszystko powinno działać. 


Jeśli już piramidka pojawiła się nam na ekrenie możemy wyczyścić metodę `init` z całego niepotrzebnego kodu tworzącego piramidkę, a następnie 
podmienić ładowanie pliku `pyramid.obj` na `blue_marble.obj`. 