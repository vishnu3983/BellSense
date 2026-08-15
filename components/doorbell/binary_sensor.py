import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import binary_sensor
from esphome.const import CONF_ID

AUTO_LOAD = []

doorbell_ns = cg.esphome_ns.namespace("doorbell")

DoorbellBinarySensor = doorbell_ns.class_(
    "DoorbellBinarySensor",
    binary_sensor.BinarySensor,
    cg.Component,
)

CONF_SAMPLE_INTERVAL = "sample_interval"
CONF_WINDOW = "window"
CONF_PRESS_THRESHOLD = "press_threshold"
CONF_RELEASE_THRESHOLD = "release_threshold"

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(
    DoorbellBinarySensor
).extend(
{
    cv.GenerateID(): cv.declare_id(DoorbellBinarySensor),

    cv.Optional(CONF_SAMPLE_INTERVAL, default="5ms"):
        cv.positive_time_period_milliseconds,

    cv.Optional(CONF_WINDOW, default="100ms"):
        cv.positive_time_period_milliseconds,

    cv.Optional(CONF_PRESS_THRESHOLD, default=0.10):
        cv.float_range(min=0.0, max=1.0),

    cv.Optional(CONF_RELEASE_THRESHOLD, default=0.05):
        cv.float_range(min=0.0, max=1.0),
})

async def to_code(config):

    var = await binary_sensor.new_binary_sensor(config)

    await cg.register_component(var, config)

    cg.add(var.set_sample_interval(
        config[CONF_SAMPLE_INTERVAL].total_milliseconds))

    cg.add(var.set_window(
        config[CONF_WINDOW].total_milliseconds))

    cg.add(var.set_press_threshold(
        config[CONF_PRESS_THRESHOLD]))

    cg.add(var.set_release_threshold(
        config[CONF_RELEASE_THRESHOLD]))