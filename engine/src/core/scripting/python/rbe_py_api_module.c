#include "rbe_py_api_module.h"

#include "../../game_properties.h"

PyObject* rbe_py_api_configure_game(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* gameTitle;
    int windowWidth;
    int windowHeight;
    int resolutionWidth;
    int resolutionHeight;
    int targetFPS;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siiiii", rbePyApiProjectConfigureKWList, &gameTitle, &windowWidth, &windowHeight, &resolutionWidth, &resolutionHeight, &targetFPS)) {
        RBEGameProperties* gameProperties = rbe_game_props_get();
        strncpy(gameProperties->gameTitle, gameTitle, sizeof(gameProperties->gameTitle) - 1);
        gameProperties->gameTitle[sizeof(gameProperties->gameTitle) - 1] = '\0';
        gameProperties->windowWidth = windowWidth;
        gameProperties->windowHeight = windowHeight;
        gameProperties->resolutionWidth = resolutionWidth;
        gameProperties->resolutionHeight = resolutionHeight;
        gameProperties->targetFPS = targetFPS;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* PyInit_rbe_py_API(void) {
    return PyModule_Create(&rbePyAPIModDef);
}
