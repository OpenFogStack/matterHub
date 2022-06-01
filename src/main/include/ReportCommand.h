/**
 *
 */

#pragma once

#include "BindingHandler.h"
#include <app/tests/suites/commands/interaction_model/InteractionModel.h>

class ReportCommand  : public InteractionModelReports, public chip::app::ReadClient::Callback {

};