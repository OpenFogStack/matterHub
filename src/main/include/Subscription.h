/**
 *
 */
// TODO Source this
#pragma once

#include "BindingHandler.h"
#include <InteractionModel.h>

class Subscription : public InteractionModelReports,
                      public chip::app::ReadClient::Callback {
  Subscription() : InteractionModelReports(this) {}

  // TODO: Create all required fields to create and manage a subscription

  /////////// ReadClient Callback Interface /////////
  void OnAttributeData(const chip::app::ConcreteDataAttributePath &path,
                       chip::TLV::TLVReader *data,
                       const chip::app::StatusIB &status) override {
    CHIP_ERROR error = status.ToChipError();
    if (CHIP_NO_ERROR != error) {
      ChipLogError(chipTool, "Response Failure: %s", chip::ErrorStr(error));
      mError = error;
      return;
    }

    if (data == nullptr) {
      ChipLogError(chipTool, "Response Failure: No Data");
      mError = CHIP_ERROR_INTERNAL;
      return;
    }
  }

  void OnEventData(const chip::app::EventHeader &eventHeader,
                   chip::TLV::TLVReader *data,
                   const chip::app::StatusIB *status) override {
    if (status != nullptr) {
      CHIP_ERROR error = status->ToChipError();
      if (CHIP_NO_ERROR != error) {
        ChipLogError(chipTool, "Response Failure: %s", chip::ErrorStr(error));
        mError = error;
        return;
      }
    }

    if (data == nullptr) {
      ChipLogError(chipTool, "Response Failure: No Data");
      mError = CHIP_ERROR_INTERNAL;
      return;
    }
  };
  void OnError(CHIP_ERROR error) override {
    ChipLogProgress(chipTool, "Error: %s", chip::ErrorStr(error));
    mError = error;
  }
  CHIP_ERROR mError = CHIP_NO_ERROR;
};