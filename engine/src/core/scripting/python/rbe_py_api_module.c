#include "rbe_py_api_module.h"

#include "../../game_properties.h"
#include "../../utils/rbe_assert.h"
#include "../../scripting/python/py_helper.h"

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

PyObject* rbe_py_api_create_assets(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* audioSourcesList;
    PyObject* texturesList;
    PyObject* fontsList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "OOO", rbePyApiCreateAssetsKWList, &audioSourcesList, &texturesList, &fontsList)) {
        RBE_ASSERT_FMT(PyList_Check(audioSourcesList), "Passed in audio source assets are not a python list, check python api implementation...");
        RBE_ASSERT_FMT(PyList_Check(texturesList), "Passed in texture assets are not a python list, check python api implementation...");
        RBE_ASSERT_FMT(PyList_Check(fontsList), "Passed in font assets are not a python list, check python api implementation...");

        // TODO: Actually put assets into structures
        // Audio Sources
        rbe_logger_debug("audio_sources:");
        for (Py_ssize_t i = 0; i < PyList_Size(audioSourcesList); i++) {
            PyObject* pAudioSourceAsset = PyList_GetItem(audioSourcesList, i);
            RBE_ASSERT(pAudioSourceAsset != NULL);
            const char* filePath = phy_get_string_from_var(pAudioSourceAsset, "file_path");
            rbe_logger_debug("file_path = %s", filePath);
            Py_DECREF(pAudioSourceAsset);
        }
        Py_DECREF(audioSourcesList);

        // Textures
        rbe_logger_debug("textures:");
        for (Py_ssize_t i = 0; i < PyList_Size(texturesList); i++) {
            PyObject* pTextureAsset = PyList_GetItem(texturesList, i);
            RBE_ASSERT(pTextureAsset != NULL);
            const char* filePath = phy_get_string_from_var(pTextureAsset, "file_path");
            const char* wrapS = phy_get_string_from_var(pTextureAsset, "wrap_s");
            const char* wrapT = phy_get_string_from_var(pTextureAsset, "wrap_t");
            const char* filterMin = phy_get_string_from_var(pTextureAsset, "filter_min");
            const char* filterMag = phy_get_string_from_var(pTextureAsset, "filter_mag");
            rbe_logger_debug("file_path = %s, wrap_s = %s, wrap_t = %s, filter_min = %s, filter_mag = %s",
                             filePath, wrapS, wrapT, filterMin, filterMag);
            Py_DECREF(pTextureAsset);
        }
        Py_DECREF(texturesList);

        // Fonts
        rbe_logger_debug("fonts:");
        for (Py_ssize_t i = 0; i < PyList_Size(fontsList); i++) {
            PyObject* pFontAsset = PyList_GetItem(fontsList, i);
            RBE_ASSERT(pFontAsset != NULL);
            const char* filePath = phy_get_string_from_var(pFontAsset, "file_path");
            const char* uid = phy_get_string_from_var(pFontAsset, "uid");
            const int size = phy_get_int_from_var(pFontAsset, "size");
            rbe_logger_debug("file_path = %s, uid = %s, size = %d", filePath, uid, size);
            Py_DECREF(pFontAsset);
        }
        Py_DECREF(fontsList);

        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_configure_inputs(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* inputActionsList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "O", rbePyApiConfigureInputsKWList, &inputActionsList)) {
        RBE_ASSERT_FMT(PyList_Check(inputActionsList), "Passed in input actions are not a python list, check python api implementation...");

        rbe_logger_debug("input actions:");
        for (Py_ssize_t i = 0; i < PyList_Size(inputActionsList); i++) {
            PyObject* pInputAction = PyList_GetItem(inputActionsList, i);
            RBE_ASSERT(pInputAction != NULL);
            const char* name = phy_get_string_from_var(pInputAction, "name");
            rbe_logger_debug("name = %s", name);
            PyObject* valuesList = PyObject_GetAttrString(pInputAction, "values");
            RBE_ASSERT(valuesList != NULL);
            RBE_ASSERT_FMT(PyList_Check(valuesList), "Input action values for '%s' is not a list!  Check python api implementation.", name);
            for (Py_ssize_t actionIndex = 0; actionIndex < PyList_Size(valuesList); actionIndex++) {
                PyObject* pActionValue = PyList_GetItem(valuesList, actionIndex);
                const char* actionValue = pyh_get_string_from_obj(pActionValue);
                rbe_logger_debug("action value = '%s'", actionValue);
            }
            Py_DECREF(pInputAction);
        }
        Py_DECREF(inputActionsList);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* PyInit_rbe_py_API(void) {
    return PyModule_Create(&rbePyAPIModDef);
}
