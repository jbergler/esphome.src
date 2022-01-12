from esphome.components import climate
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

from .. import spanet_ns, CONF_SPANET_ID, Spanet

DEPENDENCIES = ["spanet"]
CODEOWNERS = ["@jbergler"]

SpanetClimate = spanet_ns.class_("SpanetClimate", climate.Climate, cg.Component)

CONFIG_SCHEMA = cv.All(
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SpanetClimate),
            cv.GenerateID(CONF_SPANET_ID): cv.use_id(Spanet),
        }
    ).extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    parent = await cg.get_variable(config[CONF_SPANET_ID])
    cg.add(var.set_spanet_parent(parent))
