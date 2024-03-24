/*
 * Support for Flydigi Vader 3 via Bluetooth (hardware doesn't support FF in that mode).
 *
 * Learning References (tutorials/blogs):
 * https://www.nirenjan.com/2020/linux-hid-driver/
 *
 * Project References:
 * https://github.com/atar-axis/xpadneo
 *
 * See also:
 * https://github.com/atar-axis/xpadneo/pull/451 if
 */

#include <linux/delay.h>
#include <linux/hid.h>
#include <linux/module.h>

#include "vader3.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthew Carter <m@ahungry.com>");
MODULE_DESCRIPTION("Linux kernel driver for Flydigi Vader 3 via Bluetooth");
MODULE_VERSION(VADER3_VERSION);

#define VENDOR_FLYDIGI 0xD7D7
#define DEV_VADER3 0x0041

// xbox elite 2 paddles
#define BTN_PADDLES(b) (BTN_TRIGGER_HAPPY5+(b))

// vader 3 specific buttons
#define BTN_CIRCLE KEY_CAMERA
#define BTN_HOME 0x18E
#define BTN_FACE_C 0x100
#define BTN_FACE_Z 0x1C
#define BTN_BACK_LEFTMOST 0x192
#define BTN_BACK_MIDDLE_LEFT 0x179
#define BTN_BACK_MIDDLE_RIGHT 0x193
#define BTN_BACK_RIGHTMOST 0x16A

int l5_cache = 0;
int r5_cache = 0;

static int vader3_input_mapping(struct hid_device *dev,
                                struct hid_input *input,
                                struct hid_field *field,
                                struct hid_usage *usage,
                                unsigned long **bit,
                                int *max)
{
  /*
     * We are reporting the events in vader3_raw_event.
     * Skip the hid-input processing.
     */
  // return -1;

  // This uses the AUTO / underlying driver defaults - xpadneo has
  // an area where they use STATIC and redefine their own map of codes
  // to generated values - I wonder which is most appropriate to map at?
  // We can do it here, against codes (which could collide with a name brand controller),
  // or we can fiddle bits in the raw area and let them bubble up, or we can
  // remap them in the event handler (this seems easiest to get the hid-generic values out of)
  return 0;
}

static int vader3_input_configured(struct hid_device *dev,
                                   struct hid_input *input)
{
    struct input_dev * input_dev = input->input;
    int i;
    int max_btn = 16;
    int abs_min = -128;
    int abs_max = 127;
    int deadzone = 25; // Just hardcode to the same approximated value

    hid_set_drvdata(dev, input_dev);

    // Define features
    set_bit(EV_KEY, input_dev->evbit);
    set_bit(EV_ABS, input_dev->evbit);

    // define xbox buttons
    set_bit(BTN_A, input_dev->keybit);
    set_bit(BTN_B, input_dev->keybit);
    set_bit(BTN_X, input_dev->keybit);
    set_bit(BTN_Y, input_dev->keybit);
    set_bit(BTN_TL, input_dev->keybit);
    set_bit(BTN_TR, input_dev->keybit);
    set_bit(BTN_SELECT, input_dev->keybit);
    set_bit(BTN_START, input_dev->keybit);
    set_bit(BTN_THUMBL, input_dev->keybit);
    set_bit(BTN_THUMBR, input_dev->keybit);
    set_bit(BTN_MODE, input_dev->keybit);

    // Define stick capabilities
    set_bit(ABS_X, input_dev->absbit);
    set_bit(ABS_Y, input_dev->absbit);
    set_bit(ABS_Z, input_dev->absbit);
    set_bit(ABS_RZ, input_dev->absbit);

    // d-pad
    set_bit(ABS_HAT0Y, input_dev->keybit);
    set_bit(ABS_HAT0X, input_dev->keybit);

    // Define stick ranges
    input_set_abs_params(input_dev, ABS_X, abs_min, abs_max, 32, deadzone);
    input_set_abs_params(input_dev, ABS_Y, abs_min, abs_max, 32, deadzone);
    input_set_abs_params(input_dev, ABS_Z, abs_min, abs_max, 32, deadzone);
    input_set_abs_params(input_dev, ABS_RZ, abs_min, abs_max, 32, deadzone);

    // set paddles
    for (int x=0; x < 4; x++) {
      set_bit(BTN_PADDLES(x), input_dev->keybit);
    }

    // initialize the entire BTN_TRIGGER_HAPPY range, even though we are explicitly mapping xbox keys above.
    // without this, steam doesn't allow back paddle remap
    for (i = 0; i < max_btn; i++) {
        set_bit(BTN_TRIGGER_HAPPY1 + i, input_dev->keybit);
    }

    return 0;
}

// I guess we can log/report on exact raw events if we want, but if we don't
// act on it, I think it passes through to the event function.
static int vader3_raw_event(struct hid_device *dev,
                            struct hid_report *report, u8 *data, int len)
{
  /* struct input_dev *input_dev = hid_get_drvdata(dev); */
  /* hid_info(input_dev, "raw_event data 0: %d\n", data[0]); */
  /* hid_info(input_dev, "raw_event data 1: %d\n", data[1]); */
  /* hid_info(input_dev, "raw_event data 2: %d\n", data[2]); */
  /* hid_info(input_dev, "raw_event data 3: %d\n", data[3]); */
  /* hid_info(input_dev, "raw_event data 4: %d\n", data[4]); */
  /* hid_info(input_dev, "raw_event data 5: %d\n", data[5]); */
  /* hid_info(input_dev, "raw_event data 6: %d\n", data[6]); */
  /* hid_info(input_dev, "raw_event data 7: %d\n", data[7]); */
  /* hid_info(input_dev, "raw_event data 8: %d\n", data[8]); */
  /* hid_info(input_dev, "raw_event data 9: %d\n", data[9]); */
  /* hid_info(input_dev, "raw_event data 10: %d\n", data[10]); */
  /* hid_info(input_dev, "raw_event data 11: %d\n", data[11]); */
  /* hid_info(input_dev, "raw_event data 12: %d\n", data[12]); */
  /* hid_info(input_dev, "raw_event data 13: %d\n", data[13]); */
  /* hid_info(input_dev, "raw_event data 14: %d\n", data[14]); */
  /* hid_info(input_dev, "raw_event data 15: %d\n", data[15]); */
  /* hid_info(input_dev, "raw_event data 16: %d\n", data[16]); */

  // Fix the rightmost back button, the others are 8/16/32, and 64 makes it recognized
  // If we don't do this, it actually won't be detected by qjoypad
  // The C/Z are 1/2 - no idea why 4 is problematic
  if (len > 11 && data[11] & 4)
    {
      data[11] |= 64;
      data[11] &= ~4;
    }

  // C-button reports as BTN_0
  /* if (data[11] == 1) */
  /*   { */
  /*     input_report_key(input_dev, BTN_C, 1); */
  /*   } */

  // Z-button reports as Return
  /* if (data[11] == 2) */
  /*   { */
  /*     data[11] = 0; */
  /*     data[12] = 1; */
  /*   } */

  /* input_sync(input_dev); */

  return 0;

  // Sample from the tutorial, auto-mapping a bunch of stuff

    /* struct input_dev *input_dev = hid_get_drvdata(dev); */
    /* int ret = 0; */
    /* int btn; */
    /* int idx; */
    /* int num_buttons = 16; */

    /* // Samples of how we can overwrite this to anything we want. */
    /* input_report_abs(input_dev, ABS_Z, data[4]); */
    /* input_report_abs(input_dev, ABS_RX, data[5]); */
    /* input_report_abs(input_dev, ABS_RY, data[6]); */
    /* input_report_abs(input_dev, ABS_MISC, data[7]); */

    /* /\* ... *\/ */
    /* for (int i = 0; i < num_buttons; i++) */
    /*   { */
    /*     idx = 8 + (i / BITS_PER_BYTE); */
    /*     btn = !!(data[idx] & (1 << (i % BITS_PER_BYTE))); */
    /*     input_report_key(input_dev, BTN_TRIGGER_HAPPY + i, btn); */
    /*   } */

    /* input_sync(input_dev); */
    /* return ret; */
}

static int vader3_event(struct hid_device *dev, struct hid_field *field,
			 struct hid_usage *usage, __s32 value)
{
  struct input_dev *input_dev = hid_get_drvdata(dev);



  // d-pad is all messed up
  // up/down:
  //   type 3 (EV_ABS), code 17 (ABS_HAT0Y), value -1 (up), 1 (down), and 0 unpressed
  // left/right:
  //   type 3 (EV_ABS), code 16 (ABS_HAT0X), value -1 (left), 1 (right), and 0 unpressed
  // Some users complain about multiple directions inputs registering at once, I bet
  // we could track that state here and ensure it's not possible to press left and right
  // at the same time

  if (usage->type == EV_ABS)
    {
      switch (usage->code)
        {
        case ABS_HAT0X: // all d-pad comes through as this
          switch (value)
            {
            case 1: // up
              input_report_abs(input_dev, ABS_HAT0Y, -1);
              break;

            case 5: // down
              input_report_abs(input_dev, ABS_HAT0Y, 1);
              break;

            case 7: // left
              input_report_abs(input_dev, ABS_HAT0X, -1);
              break;

            case 3: // right
              input_report_abs(input_dev, ABS_HAT0X, 1);
              break;

            case 2: // up-right
              input_report_abs(input_dev, ABS_HAT0Y, -1);
              input_report_abs(input_dev, ABS_HAT0X, 1);
              break;

            case 4: // bottom-right
              input_report_abs(input_dev, ABS_HAT0Y, 1);
              input_report_abs(input_dev, ABS_HAT0X, 1);
              break;

            case 6: // bottom-left
              input_report_abs(input_dev, ABS_HAT0Y, 1);
              input_report_abs(input_dev, ABS_HAT0X, -1);
              break;

            case 8: // up-left
              input_report_abs(input_dev, ABS_HAT0Y, -1);
              input_report_abs(input_dev, ABS_HAT0X, -1);
              break;

            case 0: // no direction
              input_report_abs(input_dev, ABS_HAT0X, 0);
              input_report_abs(input_dev, ABS_HAT0Y, 0);
              break;

            default:
              hid_info(input_dev, "Unknown d-pad direction: %d\n", value);
              // input_report_abs(input_dev, usage->code, value);
              break;

            }
          break;

        default:
          // If we want Linux Gamepad Specification, we can multiply the value here to 32k
          // It seems to show up as that in jstest already?
          input_report_abs(input_dev, usage->code, value);
          break;
        }
      return 1;
    }

  if (usage->type == EV_KEY)
    {
      // elite paddle mappings
      // rightmost -> R4 -> BTN_TRIGGER_HAPPY5
      // middle right -> R5 -> BTN_TRIGGER_HAPPY6
      // leftmost -> L4 -> BTN_TRIGGER_HAPPY7
      // middle left -> L5 -> BTN_TRIGGER_HAPPY8

      /**
       * Everytime any button is pressed, there are events fired for every button on the controller with its current value.
       * Since I would like to map C/Z to the same button as L4/R4 (middle back buttons) we need to cache their value and OR
       * it with the value of the respective back paddles
       *
       * If we don't cache the value, then it will always assume the value of the back paddles, since their usage->code is higher they are
       * always processed after.
      */
      if (usage->code == BTN_FACE_C) {
        l5_cache = value;
      }

      if (usage->code == BTN_FACE_Z) {
        r5_cache = value;
      }

      int res;

      switch (usage->code)
        {
          // normal XBOX controller inputs
          case BTN_A:
          case BTN_B:
          case BTN_X:
          case BTN_Y:
          case BTN_TL:
          case BTN_TR:
          case BTN_START:
          case BTN_SELECT:
          case BTN_TL2:
          case BTN_TR2:
          case BTN_THUMBL:
          case BTN_THUMBR:
            input_report_key(input_dev, usage->code, value);
            break;

          // make the circle button the "xbox" button, steamdeck uses hold home for some functions
          // and v3p will power down if you hold home
          case BTN_CIRCLE:
            input_report_key(input_dev, BTN_MODE, value);
            break;

          case BTN_HOME: // disable the home button... could remap to another function (espace key maybe?)
            // input_report_key(input_dev, BTN_SHARE, value);
            break;

          case BTN_BACK_RIGHTMOST:
            input_report_key(input_dev, BTN_PADDLES(0), value);
            break;

          case BTN_FACE_Z: // map Z to mirror middle right since it is hard to reach
          case BTN_BACK_MIDDLE_RIGHT:
            res = r5_cache | value;
            input_report_key(input_dev, BTN_PADDLES(1), res);
            break;

          case BTN_BACK_LEFTMOST:
            input_report_key(input_dev, BTN_PADDLES(2), value);
            break;

          case BTN_FACE_C: // map C to mirror middle right since it is hard to reach
          case BTN_BACK_MIDDLE_LEFT:
            res = l5_cache | value;
            input_report_key(input_dev, BTN_PADDLES(3), res);
            break;

          default:
            // don't pass through other events
            // input_report_key(input_dev, usage->code, value);
            break;
        }

      // clear the value in the cache after processing the back paddles, this will get reset again by C/Z above on next event
      if (usage->code == BTN_BACK_MIDDLE_LEFT) {
        l5_cache = 0;
      }

      if (usage->code == BTN_BACK_MIDDLE_RIGHT) {
        r5_cache = 0;
      }

      return 1;
    }

  return 0;
}
static int vader3_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
  // mocking an xbox elite 2 controller
  hdev->product = 0x0B05;
  hdev->vendor = 0x045E;
  hdev->version = 0x903;

  int ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		return ret;
	}

	return 0;
}

static void vader3_remove(struct hid_device *hdev)
{
	hid_hw_close(hdev);
    hdev->product = DEV_VADER3;
    hdev->vendor = VENDOR_FLYDIGI;
	hid_hw_stop(hdev);
}

static const struct hid_device_id vader3_devices[] = {
  { HID_BLUETOOTH_DEVICE(VENDOR_FLYDIGI, DEV_VADER3) },
  {}
};

MODULE_DEVICE_TABLE(hid, vader3_devices);

static struct hid_driver vader3_driver = {
  .name = "vader3",
  .id_table = vader3_devices,
  .input_mapping = vader3_input_mapping,
  .input_configured = vader3_input_configured,
  .raw_event = vader3_raw_event,
  .event = vader3_event,
  .probe = vader3_probe,
  .remove = vader3_remove,
};

// module_hid_driver(vader3_driver);
static int __init vader3_init(void)
{
 	pr_info("loaded hid-vader3 %s\n", VADER3_VERSION);
	dbg_hid("vader3:%s\n", __func__);

  int ret = hid_register_driver(&vader3_driver);

	return ret;
}

static void __exit vader3_exit(void)
{
  dbg_hid("vader3:%s\n", __func__);
  hid_unregister_driver(&vader3_driver);
}

module_init(vader3_init);
module_exit(vader3_exit);
