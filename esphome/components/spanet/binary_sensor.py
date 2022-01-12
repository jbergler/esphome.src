import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID
from . import SpaNETComponent, CONF_SPANET_ID

CONF_HEATING = "heating"

TYPES = [
    CONF_HEATING,
]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_SPANET_ID): cv.use_id(SpaNETComponent),
            cv.Optional(CONF_HEATING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
                {
                    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                }
            ),
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


async def setup_conf(config, key, hub):
    if key in config:
        conf = config[key]
        sens = cg.new_Pvariable(conf[CONF_ID])
        await binary_sensor.register_binary_sensor(sens, conf)
        cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SPANET_ID])
    for key in TYPES:
        await setup_conf(config, key, hub)
