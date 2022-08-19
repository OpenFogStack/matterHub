package com.matterhub.server.entities.matter.generated.onoff;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.IntNode;
import com.matterhub.server.entities.matter.Attribute;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Command;
import com.matterhub.server.entities.metrics.DCMDCommandMetric;
import com.matterhub.server.entities.metrics.DCMDWriteMetric;
import com.matterhub.server.entities.metrics.DataType;
import com.matterhub.server.entities.metrics.Metric;
import lombok.ToString;
import org.eclipse.ditto.json.JsonField;
import org.eclipse.ditto.json.JsonValue;

import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

enum StartUpOnOff {
    Off(0x00), On(0x01), TogglePreviousOnOff(0x02);
    final byte value;

    StartUpOnOff(int value) {
        this.value = (byte) value;
    }

    static StartUpOnOff fromValue(int value) {
        for (StartUpOnOff variant : StartUpOnOff.values()) {
            if (variant.value == value) {
                return variant;
            }
        }
        throw new IllegalArgumentException("No such enum variant");
    }
}

enum OnOffEffectIdentifier {
    DelayedAllOff(0x00), DyingLight(0x01);
    final byte value;

    OnOffEffectIdentifier(int value) {
        this.value = (byte) value;
    }

    static OnOffEffectIdentifier fromValue(int value) {
        for (OnOffEffectIdentifier variant : OnOffEffectIdentifier.values()) {
            if (variant.value == value) {
                return variant;
            }
        }
        throw new IllegalArgumentException("No such enum variant");
    }
}

enum OnOffDelayedAllOffEffectVariant {
    DelayedAllOff(0x00), DyingLight(0x01);
    final byte value;

    OnOffDelayedAllOffEffectVariant(int value) {
        this.value = (byte) value;
    }

    static OnOffDelayedAllOffEffectVariant fromValue(int value) {
        for (OnOffDelayedAllOffEffectVariant variant : OnOffDelayedAllOffEffectVariant.values()) {
            if (variant.value == value) {
                return variant;
            }
        }
        throw new IllegalArgumentException("No such enum variant");
    }
}
@ToString
public class OnOffCluster
        extends Cluster {

    private final Set<Attribute> attributes;
    private final Set<Command> commands;

    public OnOffCluster(List<Integer> attributeIds, List<Integer> commandIds, Integer filler) {
        this.attributes = attributeIds.stream().map(this::getAttribute).filter(Objects::nonNull).collect(Collectors.toSet());
        // TODO assert that it contains the required attributes
        this.commands = commandIds.stream().map(this::getCommand).collect(Collectors.toSet());
        // TODO assert that it contains the required commands
    }

    public OnOffCluster(List<String> attributeNames, List<String> commandNames, String filler) {
        this.attributes = attributeNames.stream().map(this::getAttribute).collect(Collectors.toSet());
        // TODO assert that it contains the required attributes
        this.commands = commandNames.stream().map(this::getCommand).collect(Collectors.toSet());
        // TODO assert that it contains the required commands
    }

    @Override
    public int Id() {
        return 6;
    }

    @Override
    public String Name() {
        return "onoff";
    }

    @Override
    public Set<Attribute> Attributes() {
        return attributes;
    }

    @Override
    public Set<Command> Commands() {
        return commands;
    }

    @Override
    public Attribute getAttribute(int id) {
        switch (id) {
            case 0x0000: {
                var attr = new OnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4000: {
                var attr = new GlobalSceneControlAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4001: {
                return new OnTimeAttribute(this);
            }
            case 0x4002: {
                var attr = new OffWaitTimeAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4003: {
                var attr = new StartUpOnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }
        }
        if (0XFFF8 <= id && id <= 0XFFFD) {
            System.out.println("Ignoring global attributes");
            return null;
        }
        throw new IllegalArgumentException("Unknown attribute id: " + id);
    }

    @Override
    public Attribute getAttribute(String name) {
        switch (name) {
            case "OnOff": {
                var attr = new OnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "GlobalSceneControl": {
                var attr = new GlobalSceneControlAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "OnTime": {
                return new OnTimeAttribute(this);
            }
            case "OffWaitTime": {
                var attr = new OffWaitTimeAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "StartUpOnOff": {
                var attr = new StartUpOnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }

        }
        throw new IllegalArgumentException("Unknown attribute id: " + name);
    }

    @Override
    public Command getCommand(int id) {
        switch (id) {
            case 0x00: {
                return new OffCommand(this);
            }
            case 0x01: {
                return new OnCommand(this);
            }
            case 0x02: {
                var cmd = new ToggleCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x40: {
                var cmd = new OffWithEffectCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x41: {
                var cmd = new OnWithRecallGlobalSceneCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x42: {
                var cmd = new OnWithTimedOffCommand();
                cmd.parentCluster = this;
                return cmd;
            }

        }
        throw new IllegalArgumentException("Unknown attribute id: " + id);
    }

    @Override
    public Command getCommand(String name) {
        switch (name) {
            case "Off": {
                return new OffCommand(this);
            }
            case "On": {
                return new OnCommand(this);
            }
            case "Toggle": {
                var cmd = new ToggleCommand();
                cmd.parentCluster = this;
                return cmd;
            }
        }
        throw new IllegalArgumentException("Unknown attribute id: " + name);
    }
}
@ToString
class OnOffAttribute
        implements Attribute {

    boolean state;
    @ToString.Exclude Cluster parentCluster;

    @Override
    public int Id() {
        return 0x0000;
    }

    @Override
    public String Name() {
        return "OnOff";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public void fromMatterValue(JsonNode matterValue) {
        this.state = matterValue.asBoolean();
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public Optional<Metric> fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        boolean newState = featureValue.getValue().asBoolean();
        if (newState == this.state) {
            return Optional.empty();
        }
        this.state = newState;
        if (state) {
            return Optional.of(new OnCommand(this.parentCluster).toMetric());
        } else {
            return Optional.of(new OffCommand(this.parentCluster).toMetric());
        }
    }
}
@ToString
class GlobalSceneControlAttribute
        implements Attribute {
    @ToString.Exclude Cluster parentCluster;
    boolean state;

    @Override
    public int Id() {
        return 0x4000;
    }

    @Override
    public String Name() {
        return "GlobalSceneControl";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public void fromMatterValue(JsonNode matterValue) {
        this.state = matterValue.asBoolean();
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public Optional<Metric> fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = featureValue.getValue().asBoolean();
        return Optional.empty();
    }

}
@ToString
class OnTimeAttribute
        implements Attribute {
    @ToString.Exclude private final Cluster parentCluster;
    private short state;

    OnTimeAttribute(Cluster parentCluster) {
        this.parentCluster = parentCluster;
    }

    @Override
    public int Id() {
        return 0x4001;
    }

    @Override
    public String Name() {
        return "OnTime";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public void fromMatterValue(JsonNode matterValue) {
        this.state = (short) matterValue.asInt();
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public Optional<Metric> fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        short newState = (short) featureValue.getValue().asInt();
        if (newState == this.state) {
            return Optional.empty();
        }
        this.state = newState;
        //Writable true
        return Optional.of(new DCMDWriteMetric(parentCluster.Parent().Id(), parentCluster.Id(), this.Id(), System.currentTimeMillis() / 1000L, DataType.UInt16, new IntNode(this.state)));
    }

}
@ToString
class OffWaitTimeAttribute
        implements Attribute {
    @ToString.Exclude Cluster parentCluster;
    short state;

    @Override
    public int Id() {
        return 0x4002;
    }

    @Override
    public String Name() {
        return "OffWaitTime";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }


    @Override
    public void fromMatterValue(JsonNode matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public Optional<Metric> fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = (short) featureValue.getValue().asInt();
        //Writable true
        return Optional.empty();
    }

}
@ToString
class StartUpOnOffAttribute
        implements Attribute {
    @ToString.Exclude Cluster parentCluster;
    StartUpOnOff state = StartUpOnOff.Off;

    @Override
    public int Id() {
        return 0x4003;
    }

    @Override
    public String Name() {
        return "StartUpOnOff";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public void fromMatterValue(JsonNode matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state.value));
    }

    @Override
    public Optional<Metric> fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = StartUpOnOff.fromValue(featureValue.getValue().asInt());
        return Optional.empty();
    }
}

class OffCommand
        implements Command {
    private final Cluster parentCluster;

    OffCommand(Cluster parentCluster) {
        this.parentCluster = parentCluster;
    }

    @Override
    public int Id() {
        return 0x00;
    }

    @Override
    public String Name() {
        return "Off";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return new DCMDCommandMetric(parentCluster.Parent().Id(), parentCluster.Id(), this.Id(), System.currentTimeMillis() / 1000L, DataType.None, null);
    }
}

class OnCommand
        implements Command {
    private final Cluster parentCluster;

    OnCommand(Cluster parentCluster) {
        this.parentCluster = parentCluster;
    }

    @Override
    public int Id() {
        return 0x01;
    }

    @Override
    public String Name() {
        return "On";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return new DCMDCommandMetric(parentCluster.Parent().Id(), parentCluster.Id(), this.Id(), System.currentTimeMillis() / 1000L, DataType.None, null);
    }
}

class ToggleCommand
        implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x02;
    }

    @Override
    public String Name() {
        return "Toggle";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

class OffWithEffectCommand
        implements Command {
    Cluster parentCluster;
    OnOffEffectIdentifier effectId;
    OnOffDelayedAllOffEffectVariant effectVariant;

    OffWithEffectCommand() {
    }

    OffWithEffectCommand(OnOffEffectIdentifier effectId, OnOffDelayedAllOffEffectVariant effectVariant) {
        this.effectId = effectId;
        this.effectVariant = effectVariant;
    }

    @Override
    public int Id() {
        return 0x40;
    }

    @Override
    public String Name() {
        return "OffWithEffect";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

//TODO create constructor
class OnWithRecallGlobalSceneCommand
        implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x41;
    }

    @Override
    public String Name() {
        return "OnWithRecallGlobalScene";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

//TODO create constructor
class OnWithTimedOffCommand
        implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0;
    }

    @Override
    public String Name() {
        return null;
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}
