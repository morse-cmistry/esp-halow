idf_component_register(
    SRCS
	"app_main.c"
	"halow.c"
	"server.c"
    INCLUDE_DIRS "."
    REQUIRES halow nvs_flash esp_wifi esp_http_server
)