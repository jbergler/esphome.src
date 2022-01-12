import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    ICON_THERMOMETER,
)
from . import SpaNETComponent, CONF_SPANET_ID

CONF_WATER_TEMPERATURE = "water_temperature"
CONF_SET_TEMPERATURE = "set_temperature"

TYPES = [
    CONF_WATER_TEMPERATURE,
    CONF_SET_TEMPERATURE,
]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_SPANET_ID): cv.use_id(SpaNETComponent),
            cv.Optional(CONF_WATER_TEMPERATURE): sensor.sensor_schema(
                UNIT_CELSIUS,
                ICON_THERMOMETER,
                0,
                DEVICE_CLASS_TEMPERATURE,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SET_TEMPERATURE): sensor.sensor_schema(
                UNIT_CELSIUS,
                ICON_THERMOMETER,
                0,
                DEVICE_CLASS_TEMPERATURE,
                STATE_CLASS_MEASUREMENT,
            ),
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


async def setup_conf(config, key, hub):
    if key in config:
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(hub, f"set_{key}_sensor")(sens))


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SPANET_ID])
    for key in TYPES:
        await setup_conf(config, key, hub)
